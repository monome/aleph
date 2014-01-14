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

s32 scaler_note_val(void* scaler, io_t in) {
  /* print_dbg("\r\n requesting note_scaler value for input: 0x"); */
  /* print_dbg_hex((u32)in); */
  //  u16 uin = BIT_ABS_16((s16)in);
  if(in < 0) { in = 0; }
  return tabVal[(u16)((u16)in >> inRshift)];
}

void scaler_note_str(char* dst, void* scaler,  io_t in) {
  u16 uin = BIT_ABS_16((s16)in) >> inRshift;
  // top 7 bits are semitones (== midi note number)
  // low 3 bits are microtuning
  //// FIXME: print number.tune instead of hz ?
  print_fix16(dst, tabVal[(u16)uin] );
}

// init function
void scaler_note_init(void* scaler) {
  //  ParamScaler* sc = (ParamScaler*)scaler;
  // check descriptor
  /* if( sc->desc->type != eParamTypeNote) { */
  /*   print_dbg("\r\n !!! warning: wrong param type for note scaler"); */
  /* } */
  
  // init flag for static data
  if(initFlag) { 
    ;;
  } else {
    initFlag = 1;
    // assign
    tabVal = scaler_get_nv_data(eParamTypeNote);

    /* // allocate */
    /* print_dbg("\r\n allocating static memory for note scalers"); */
    /* tabVal = (s32*)alloc_mem(tabSize * 4); */
        
    /* // load gain data */
    /* print_dbg("\r\n loading gain scaler data from sdcard"); */
    /* files_load_scaler_name("scaler_note_val.dat", tabVal, tabSize); */
    /* print_dbg("\r\n finished loading note scaler data from files."); */
  }

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
io_t scaler_note_in(void* scaler, s32 x) {
  // value table is monotonic, can binary search
  s32 jl = 0;
  s32 ju = tabSize - 1;
  s32 jm;

  print_dbg("\r\n scaler_note_in, x: 0x");
  print_dbg_hex(x);

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
s32 scaler_note_inc(void* sc, io_t* pin, io_t inc ) {
  static const io_t incMul = 1 << (IO_BITS - tabBits);
  // multiply by smallest significant abcissa
  inc *= incMul;
  // use saturation
  *pin = op_sadd(*pin, inc);
  if(*pin < 0) { *pin = 0; }
  // scale and return.
  // ignoring ranges in descriptor at least for now.
  return scaler_note_val(sc, *pin);
}
