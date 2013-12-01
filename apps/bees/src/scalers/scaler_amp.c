// asf
#include "print_funcs.h"
// aleph-avr32
#include "memory.h"
// bees
#include "files.h"
#include "param_scaler.h"
#include "scaler_amp.h"

// table size
static const u8 tabBits = 10;
static const u32 tabSize = 1024;
// shift from io_t size to index
static const u8 inRshift = 5;

static s32* tabVal;
static s32* tabRep;

static u8 initFlag = 0;

//-------------------
//--- static funcs


//-----------------------
//---- extern funcs

s32 scaler_amp_val(void* scaler, io_t in) {
  /* print_dbg("\r\n requesting amp_scaler value for input: 0x"); */
  /* print_dbg_hex((u32)in); */
  u16 uin = BIT_ABS_16((s16)in);
  return tabVal[(u16)((u16)uin >> inRshift)];
}

void scaler_amp_str(char* dst, void* scaler,  io_t in) {
  /* print_dbg("\r\n requesting amp_scaler representation for input: 0x"); */
  /* print_dbg_hex((u32)in); */

  //  in >>= inRshift;
  u16 uin = BIT_ABS_16((s16)in) >> inRshift;

  if(uin == 0) {
    strcpy(dst, "   -inf");
  } else if (uin == (tabSize - 1)) {
    print_fix16(dst, 0);
  } else {
    print_fix16(dst, tabRep[(u16)uin] );
  }

}

// init function
void scaler_amp_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;
  // check descriptor
  if( sc->desc->type != eParamTypeAmp) {
    print_dbg("\r\n !!! warning: wrong param type for amp scaler");
  }
  
  // init flag for static data
  if(initFlag) { 
    ;;
  } else {
    initFlag = 1;
    // allocate
    print_dbg("\r\n allocating static memory for amp scalers");
    tabVal = (s32*)alloc_mem(tabSize * 4);
    tabRep = (s32*)alloc_mem(tabSize * 4);
    
    // load gain data
    print_dbg("\r\n loading gain scaler data from sdcard");
    files_load_scaler_name("scaler_amp_val.bin", tabVal, tabSize);
    files_load_scaler_name("scaler_amp_rep.bin", tabRep, tabSize);
   print_dbg("\r\n finished loading amp scaler data from files.");
  }

  /// FIXME: should consider requested param range,
  //  and compute a customized multiplier here if necessary.

  /// proper class-based initialization was breaking for some reason, 
  // driving me insane.
  // so for now, scaling functions are static.>.????

  //// FIXME: add tuning functions (???)
  //  sc->tune = NULL;
  //  sc->numTune = 0;  
}


// get input given DSP value (use sparingly)
io_t scaler_amp_in(void* scaler, s32 x) {
  // value table is monotonic, can binary search
  s32 jl = 0;
  s32 ju = tabSize - 1;
  s32 jm;
  while(ju - jl > 1) {
    jm = (ju + jl) >> 1;
    if(x >= tabVal[jm]) {
      jl = jm;
    } else {
      ju = jm;
    }
  }
  return (u16)jm << inRshift;
}


// increment input by pointer, return value
s32 scaler_amp_inc(void* sc, io_t* pin, io_t inc ) {
  static const io_t incMul = 1 << (IO_BITS - tabBits);
  // multiply by smallest significant abcissa
  inc *= incMul;
  // use saturation
  *pin = op_sadd(*pin, inc);
  // scale and return.
  // ignoring ranges in descriptor at least for now.
  return scaler_amp_val(sc, *pin);
}
