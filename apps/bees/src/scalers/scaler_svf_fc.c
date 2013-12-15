// std
#include <string.h>
// asf
#include "print_funcs.h"
// aleph-avr32
#include "memory.h"
// bees
#include "files.h"
#include "param_scaler.h"
#include "scaler_note.h"
#include "scaler_svf_fc.h"

// table size
static const u8 tabBits = 10;
static const u32 tabSize = 1024;
// shift from io_t size to index
static const u8 inRshift = 5;

static const s32* tabVal;

static u8 initFlag = 0;

//-------------------
//--- static funcs

//-----------------------
//---- extern funcs

s32 scaler_svf_fc_val(void* scaler, io_t in) {
  /* print_dbg("\r\n requesting svf_fc_scaler value for input: 0x"); */
  /* print_dbg_hex((u32)in); */
  if(in < 0) { in = 0; }
  return tabVal[(u16)((u16)in >> inRshift)];
}

void scaler_svf_fc_str(char* dst, void* scaler, io_t in) {
  //  u16 uin = BIT_ABS_16((s16)in) >> inRshift;
  // use note scaler class for representation
  scaler_note_str(dst, scaler, in);
}

// init function
void scaler_svf_fc_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;
  // check descriptor
  if( sc->desc->type != eParamTypeSvfFreq) {
    print_dbg("\r\n !!! warning: wrong param type for svf_fc scaler");
  }
  
  // init flag for static data
  if(initFlag) { 
    ;;
  } else {
    initFlag = 1;

    // assign
    tabVal = scaler_get_nv_data(eParamTypeSvfFreq);

    // allocate
    /* print_dbg("\r\n allocating static memory for svf_fc scalers"); */
    /* tabVal = (s32*)alloc_mem(tabSize * 4); */
        
    /* // load gain data */
    /* print_dbg("\r\n loading gain scaler data from sdcard"); */
    /* files_load_scaler_name("scaler_svf_fc_val.dat", tabVal, tabSize); */
    /* print_dbg("\r\n finished loading svf_fc scaler data from files."); */
  }

  // hack:
  // use the 16.16 table from hz scaler for representation.
  // make sure note scaler is also initialized.
  scaler_note_init(NULL);

  /// FIXME: should consider requested param range,
  //  and compute a customized multiplier here if necessary.

  /// proper class-based initialization was breaking for some reason, 
  // driving me insane.
  // so for now, scaling functions are in a static array in param_scaler.c

  //// FIXME: add tuning functions (???)
  //  sc->tune = NULL;
  //  sc->numTune = 0;  
}

// get input given DSP value (use sparingly)
io_t scaler_svf_fc_in(void* scaler, s32 x) {
  // value table is monotonic, can binary search
  s32 jl = 0;
  s32 ju = tabSize - 1;
  s32 jm;

  print_dbg("\r\n scaler_svf_fc_in, x: 0x");
  print_dbg_hex(x);

  // binary tree search
  while(ju - jl > 1) {
    jm = (ju + jl) >> 1;
    // value table is always ascending
    if(x >= tabVal[jm]) {
      jl = jm;
    } else {
      ju = jm;
    }
  }

  return (u16)jm << inRshift;
}


// increment input by pointer, return value
s32 scaler_svf_fc_inc(void* sc, io_t* pin, io_t inc ) {
  static const io_t incMul = 1 << (IO_BITS - tabBits);
  // multiply by smallest significant abcissa
  inc *= incMul;
  // use saturation
  *pin = op_sadd(*pin, inc);
  if(*pin < 0) { *pin = 0; }
  // scale and return.
  // ignoring ranges in descriptor at least for now.
  return scaler_svf_fc_val(sc, *pin);
}
