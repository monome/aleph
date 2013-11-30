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
  print_dbg("\r\n init fixed-point linear scaler");
  //  ParamScaler* sc = (ParamScaler*)scaler;
  // nothing to do
}

s32 scaler_fix_val(void* scaler, io_t in) {
  s32 norm = in << (32 - IO_BITS);

  print_dbg("\r\n linear-fixed scaler, get value; input: 0x");
  print_dbg_hex(in);

  print_dbg(", normalized: 0x");
  print_dbg_hex(norm);

  // normalize
  return norm;
}

void scaler_fix_str(char* dst, void* scaler, io_t in) {
  // first normalize
  s32 norm = in << (32 - IO_BITS);
  u8 r = ((ParamScaler*)scaler)->desc->radix;

  print_dbg("\r\n linear-fixed scaler, get string; input: 0x");
  print_dbg_hex(in);

  print_dbg(" , normalized: 0x");
  print_dbg_hex(norm);


  // rshift back for display, depending on radix
  if(r < 16) {
    norm >>= (16 - r);
  }

  print_dbg(" , adjusted for radix: 0x");
  print_dbg_hex(norm);


  print_fix16(dst, norm);
}

io_t scaler_fix_in(void* scaler, s32 val) {
  // de-normalize
  return val >> (32 - IO_BITS);
}
