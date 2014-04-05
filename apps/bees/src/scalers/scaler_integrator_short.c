// std
#include <string.h>
// asf
#include "print_funcs.h"
// aleph-avr32
#include "memory.h"
// bees
#include "files.h"
#include "param_scaler.h"
#include "scaler_integrator_short.h"

// table size
static const u8 tabBits = 10;
static const u32 tabSize = 1024;
// shift from io_t size to index
static const u8 inRshift = 5;

static const s32* tabVal;
//static const s32* tabRep;

static u8 initFlag = 0;

//-------------------
//--- static funcs


//-----------------------
//---- extern funcs

s32 scaler_integrator_short_val(void* scaler, io_t in) {
  if(in < 0) { in = 0; }
  return tabVal[(u16)((u16)in >> inRshift)];
}

void scaler_integrator_short_str(char* dst, void* scaler,  io_t in) {
  //  u16 uin = (in < 0) ? 0 : ((u16)in >> inRshift) ;
  /* print_dbg("\r\n ingegrator_str() , input: 0x"); */
  /* print_dbg_hex(in); */
  /* print_dbg(" , index : 0x"); */
  /* print_dbg_hex(uin); */
  /* print_dbg(" , val : 0x"); */
  /* print_dbg_hex(tabRep[uin]); */

  ///// HACK:
  // trying non-table solution... slow :S
  /// in this test, using known magic-number multiplier.
  /// should compute from descriptor in init.
  //  print_dbg(" , computed val : 0x");
  if(in < 0) { in = 0; }
  //  print_dbg_hex(fix16_mul((s32)in << 1, 0x400000) );
  print_fix16(dst, fix16_mul((s32)in << 1, 0x400000) );
  
  //  print_fix16(dst, tabRep[uin] );
}

// init function
void scaler_integrator_short_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;


  print_dbg("\r\n initializing integrator_short scaler for param, label: ");
  print_dbg(sc->desc->label);

  // check descriptor
  if( sc->desc->type != eParamTypeIntegratorShort) {
    print_dbg("\r\n !!! warning: wrong param type for integrator_short scaler");
    print_dbg(" ; this param has type: ");
    print_dbg_ulong(sc->desc->type);
  }
  
  // init flag for static data
  if(initFlag) { 
    ;;
  } else {
    initFlag = 1;
    // assign
    tabVal = scaler_get_nv_data(eParamTypeIntegratorShort);
  }

  sc->inMin = 0;
  sc->inMax = (tabSize - 1) << inRshift;


  //// FIXME: add tuning functions....
  /// here, that would mean adjusting for actual samplerate. 
  /// table data assumes 48k.
  //  sc->tune = NULL;
  //  sc->numTune = 0;  
}


// get input given DSP value (use sparingly)
io_t scaler_integrator_short_in(void* scaler, s32 x) {
  // value table is monotonic, can binary search
  s32 jl = 0;
  s32 ju = tabSize - 1;
  s32 jm;

  print_dbg("\r\n scaler_integrator_short_in, x: 0x");
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
s32 scaler_integrator_short_inc(void* scaler, io_t* pin, io_t inc ) {
  ParamScaler* sc = (ParamScaler*)scaler;

  // use saturation
  *pin = op_sadd(*pin, inc);

  if(*pin < sc->inMin) { *pin = sc->inMin; }
  if(*pin > sc->inMax) { *pin = sc->inMax; }

  // scale and return.
  // ignoring ranges in descriptor at least for now.... :S
  return scaler_integrator_short_val(sc, *pin);
}
