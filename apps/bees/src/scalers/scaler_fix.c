// linear fixed-point value with movable radix

/*
  the param descriptor specifies a radix.
  this value is size of the whole-number range, measured in bits.
  we want input from the op network to be scaled to the full range of the parameter,
  insofar as possible.
  this means shifting it to the full 4-byte range,
  but using the radix when printing.
 */

#include "print_funcs.h"

#include "op_math.h"
#include "scaler_fix.h"

void scaler_fix_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;

  print_dbg("\r\n initializing fixed-point scaler for param, label: ");
  print_dbg(sc->desc->label);

  sc->inMin = scaler_fix_in(scaler, sc->desc->min);
  sc->inMax = scaler_fix_in(scaler, sc->desc->max);

  /* print_dbg("\r\n init fixed-point linear scaler"); */
  /* print_dbg(", input min: 0x"); */
  /* print_dbg_hex(sc->inMin); */
  /* print_dbg(", input max: 0x"); */
  /* print_dbg_hex(sc->inMax); */
}

s32 scaler_fix_val(void* scaler, io_t in) {
  s32 norm;
  ParamScaler* sc = (ParamScaler*)scaler;

  if(in > sc->inMax) { in = sc->inMax; }
  if(in < sc->inMin) { in = sc->inMin; }

  // normalize
  norm  = in << (32 - IO_BITS);

  // apply radix to put us in the correct range
  u8 r = ((ParamScaler*)scaler)->desc->radix;
  if(r < (32 - IO_BITS)) {
    norm >>= (32 - IO_BITS - r);
  }

  /* print_dbg("\r\n linear-fixed scaler, get value; input: 0x"); */
  /* print_dbg_hex(in); */
  /* print_dbg(", normalized: 0x"); */
  /* print_dbg_hex(norm); */

  // set full-scale
  return norm;
}

void scaler_fix_str(char* dst, void* scaler, io_t in) {
  s32 norm;
  ParamScaler* sc = (ParamScaler*)scaler;

  if(in > sc->inMax) { in = sc->inMax; }
  if(in < sc->inMin) { in = sc->inMin; }

  norm = in << (32 - IO_BITS);
  u8 r = ((ParamScaler*)scaler)->desc->radix;

  /* print_dbg("\r\n linear-fixed scaler, get string; input: 0x"); */
  /* print_dbg_hex(in); */
  /* print_dbg(" , normalized: 0x"); */
  /* print_dbg_hex(norm); */

  // rshift back for display, depending on radix
  if(r < 32 - IO_BITS) {
    norm >>= (32 - IO_BITS - r);
  }

  /* print_dbg(" , adjusted for radix: 0x"); */
  /* print_dbg_hex(norm); */
   print_fix16(dst, norm); 
}

io_t scaler_fix_in(void* scaler, s32 val) {
  // un-normalize
  u8 r = ((ParamScaler*)scaler)->desc->radix;
  if(r < 16) {
    val <<= (32 - IO_BITS - r);
  }

  return val >> (32 - IO_BITS);
}

// increment input by pointer, return value
s32 scaler_fix_inc(void* sc, io_t* pin, io_t inc ) {
  s32 val;
  ParamScaler* scaler = (ParamScaler*)sc;

  print_dbg("\r\n scaler_fix_inc, input: 0x");
  print_dbg_hex(*pin);
  print_dbg(" , inc: 0x");
  print_dbg_hex(inc);

  // use saturation
  *pin = op_sadd(*pin, inc);

  print_dbg(" , input sum: 0x");
  print_dbg_hex(*pin);


  // check bounds again after scaling
  val = scaler_fix_val(sc, *pin);

  print_dbg(" \r\n scaled result: 0x");
  print_dbg_hex(val);

    print_dbg("\r\n descriptor max: 0x");
    print_dbg_hex(scaler->desc->max);
      print_dbg(", min: 0x");
    print_dbg_hex(scaler->desc->min);

  if(val > scaler->desc->max)

  if(val > scaler->desc->max) {
    print_dbg("\r\n high saturation in scaler_fix_inc, value: 0x");
    print_dbg_hex(val);
    print_dbg(", max: 0x");
    print_dbg_hex(scaler->desc->max);
    *pin = scaler->inMax;
    return scaler->desc->max;
  }

  if(val < scaler->desc->min) {
    print_dbg("\r\n low saturation in scaler_fix_inc, value: 0x");
    print_dbg_hex(val);
    print_dbg(", min: 0x");
    print_dbg_hex(scaler->desc->min);
    *pin = scaler->inMin;
    return scaler->desc->min;
  }

  return val;
}
