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
  }

  // hack:
  // use the 16.16 table from hz scaler for representation.
  // make sure note scaler is also initialized.
  //.... why doesn't this always crash??? (NULL arg)
  scaler_note_init(NULL);

  sc->inMin = 0;
  sc->inMax = (tabSize - 1) << inRshift;

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
s32 scaler_svf_fc_inc(void* scaler, io_t* pin, io_t inc ) {
  ParamScaler* sc = (ParamScaler*)scaler;
  // this speeds up the knob a great deal.
#if 0
  s32 sinc;
  // scale up to smallest significant abscissa:
  // check for 16b overflow
  sinc = (s32)inc << inRshift;
  if(sinc > 0x3fff) { 
    inc = (io_t)0x3fff;
  } 
  else if (sinc < (s32)0xffffc000) { 
    inc = (io_t)0xc000;
  }
#endif
  
  // use saturation
  *pin = op_sadd(*pin, inc);

  if(*pin < sc->inMin) { *pin = sc->inMin; }
  if(*pin > sc->inMax) { *pin = sc->inMax; }

  // scale and return.
  // ignoring ranges in descriptor at least for now.
  return scaler_svf_fc_val(sc, *pin);
}
