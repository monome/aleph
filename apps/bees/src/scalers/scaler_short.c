// scaler_short.h

/*
  scaler for 16-bit signed short parameter. 
  displayed as integer part of 16.16 
 */

#include "print_funcs.h"

#include "op_math.h"
#include "scaler_short.h"


void scaler_short_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;

  // print_dbg("\r\n initializing short scaler for param, label: ");
  // print_dbg(sc->desc->label);
  sc->inMin = scaler_short_in(scaler, sc->desc->min);
  sc->inMax = scaler_short_in(scaler, sc->desc->max);

}

s32 scaler_short_val(void* scaler, io_t in) {
  return in;
}

void scaler_short_str(char* dst, void* scaler, io_t in) {
  print_fix16(dst, in << 16);
}

// get IO input value based on expected output
io_t scaler_short_in(void* scaler, s32 val) {
  return val;
}

// increment input by pointer, return value
s32 scaler_short_inc(void* sc, io_t* pin, io_t inc ) {
  s32 val;
  ParamScaler* scaler = (ParamScaler*)sc;
  // use saturation
  *pin = op_sadd(*pin, inc);
  // check bounds again after scaling
  val = scaler_short_val(sc, *pin);
  if(val > scaler->desc->max) {
    *pin = scaler->inMax;
    return scaler->desc->max;
  }
  if(val < scaler->desc->min) {
    *pin = scaler->inMin;
    return scaler->desc->min;
  }
  return val;
}
