#include "op_math.h"
#include "scaler_fract.h"

void scaler_fract_init(ParamScaler* sc, const ParamDesc* desc) {
  ;; // nothing to do
}

s32 scaler_fract_val(io_t in) {
  // io_t is 16-bit signed integer
  return in << 15;
}

void scaler_fract_str(char* dst, io_t in) {
  print_fract16(dst, in << 15);
}

io_t scaler_fract_in(s32 val) {
  return val >> 15;
}
