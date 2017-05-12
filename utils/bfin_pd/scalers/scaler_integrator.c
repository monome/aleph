// std
#include <string.h>
// asf
#include "param_scaler.h"
#include "scaler_integrator.h"

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

s32 scaler_integrator_val(void* scaler, io_t in) {
  if(in < 0) { in = 0; }
  return tabVal[(u16)((u16)in >> inRshift)];
}

void scaler_integrator_str(char* dst, void* scaler,  io_t in) {
  //  u16 uin = (in < 0) ? 0 : ((u16)in >> inRshift) ;

  ///// HACK:
  // trying non-table solution... slow :S
  /// in this test, using known magic-number multiplier.
  /// should compute from descriptor in init.
  if(in < 0) { in = 0; }
  print_fix16(dst, fix16_mul((s32)in << 1, 0x400000) );
  
  //  print_fix16(dst, tabRep[uin] );
}

// init function
void scaler_integrator_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;



  // check descriptor
  if( sc->desc->type != eParamTypeIntegrator) {
  }
  
  // init flag for static data
  if(initFlag) { 
    ;;
  } else {
    initFlag = 1;

    // assign
    tabVal = scaler_get_nv_data(eParamTypeIntegrator);
    //    tabRep = scaler_get_nv_rep(eParamTypeIntegrator);
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
io_t scaler_integrator_in(void* scaler, s32 x) {
  // value table is monotonic, can binary search
  s32 jl = 0;
  s32 ju = tabSize - 1;
  s32 jm;


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


  return (u16)jm << inRshift;
}


// increment input by pointer, return value
s32 scaler_integrator_inc(void* scaler, io_t* pin, io_t inc ) {
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
  return scaler_integrator_val(sc, *pin);
}
