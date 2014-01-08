// std
#include <string.h>
// asf
#include "print_funcs.h"
// aleph-avr32
#include "memory.h"
// bees
#include "files.h"
#include "param_scaler.h"
#include "scaler_integrator.h"

// table size
static const u8 tabBits = 10;
static const u32 tabSize = 1024;
// shift from io_t size to index
static const u8 inRshift = 5;

static const s32* tabVal;
static const s32* tabRep;

static u8 initFlag = 0;

//-------------------
//--- static funcs


//-----------------------
//---- extern funcs

s32 scaler_integrator_val(void* scaler, io_t in) {
  print_dbg("\r\n requesting amp_scaler value for input: 0x");
  print_dbg_hex((u32)in);

  if(in < 0) { in = 0; }
  return tabVal[(u16)((u16)in >> inRshift)];
}

void scaler_integrator_str(char* dst, void* scaler,  io_t in) {
  u16 uin = BIT_ABS_16((s16)in) >> inRshift;
  print_fix16(dst, tabRep[(u16)uin] );
}

// init function
void scaler_integrator_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;


  print_dbg("\r\n initializing integrator scaler for param, label: ");
  print_dbg(sc->desc->label);

  // check descriptor
  if( sc->desc->type != eParamTypeIntegrator) {
    print_dbg("\r\n !!! warning: wrong param type for integrator scaler");
    print_dbg(" ; this param has type: ");
    print_dbg_ulong(sc->desc->type);
  }
  
  // init flag for static data
  if(initFlag) { 
    ;;
  } else {
    initFlag = 1;

    // assign
    tabVal = scaler_get_nv_data(eParamTypeIntegrator);
    tabRep = scaler_get_nv_rep(eParamTypeIntegrator);

    // allocate
    //    print_dbg("\r\n allocating static memory for integrator scalers");
    //    tabVal = (s32*)alloc_mem(tabSize * 4);
    //    tabRep = (s32*)alloc_mem(tabSize * 4);
    
    // load gain data
    //    print_dbg("\r\n loading integrator scaler data from sdcard");
    //    files_load_scaler_name("scaler_integrator_val.dat", tabVal, tabSize);
    //    files_load_scaler_name("scaler_integrator_rep.dat", tabRep, tabSize);
    //   print_dbg("\r\n finished loading amp scaler data from files.");
  }

  //// FIXME: add tuning functions....
  /// here, that would mean adjusting for actual samplerate. 
  /// table data assumes 48k.
  //  sc->tune = NULL;
  //  sc->numTune = 0;  
}


// get input given DSP value (use sparingly)
io_t scaler_integrator_in(void* scaler, s32 x) {
  // value table is monotonic, can binary search
  s32 jl = 0;
  s32 ju = tabSize - 1;
  s32 jm;

  print_dbg("\r\n scaler_integrator_in, x: 0x");
  print_dbg_hex(x);

  // first, cheat and check zero.
  /// will often be true
  if(x == 0) { return 0; }

  while(ju - jl > 1) {
    jm = (ju + jl) >> 1;
    // value table is always ascending
    if(x >= tabVal[jm]) {
      jl = jm;
    } else {
      ju = jm;
    }
  }

  /* print_dbg(" , median index: "); */
  /* print_dbg_ulong(jm); */

  return (u16)jm << inRshift;
}


// increment input by pointer, return value
s32 scaler_integrator_inc(void* sc, io_t* pin, io_t inc ) {
  static const io_t incMul = 1 << (IO_BITS - tabBits);
  // multiply by smallest significant abcissa
  inc *= incMul;
  // use saturation
  *pin = op_sadd(*pin, inc);
  if(*pin < 0) { *pin = 0; }
  // scale and return.
  // ignoring ranges in descriptor at least for now.
  return scaler_integrator_val(sc, *pin);
}
