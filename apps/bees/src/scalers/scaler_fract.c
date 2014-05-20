// scaler_fract.h

/*
  scaler for 32-bit signed fract parameter. 
  top 16 bits only are displayed as fractional part of fix16
 */

#include "print_funcs.h"

#include "op_math.h"
#include "scaler_fract.h"


void scaler_fract_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;

  // print_dbg("\r\n initializing fract scaler for param, label: ");
  // print_dbg(sc->desc->label);

  sc->inMin = scaler_fract_in(scaler, sc->desc->min);
  sc->inMax = scaler_fract_in(scaler, sc->desc->max);

}

s32 scaler_fract_val(void* scaler, io_t in) {
  // normalize to 32b
  return in << IO_SHIFT_32;
}

void scaler_fract_str(char* dst, void* scaler, io_t in) {
  // print as fractional part of fix16, shifting normalized IO to 16b...
  print_fix16(dst, (s32)in);
  
}

// get IO input value based on expected output
io_t scaler_fract_in(void* scaler, s32 val) {
  return val >> IO_SHIFT_32;
}

// increment input by pointer, return value
s32 scaler_fract_inc(void* sc, io_t* pin, io_t inc ) {
  s32 val;
  ParamScaler* scaler = (ParamScaler*)sc;
  // use saturation
  *pin = op_sadd(*pin, inc);
  // check bounds again after scaling
  val = scaler_fract_val(sc, *pin);
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
