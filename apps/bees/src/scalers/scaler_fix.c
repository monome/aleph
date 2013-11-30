#include "op_math.h"
#include "scaler_fix.h"

void scaler_fix_init(ParamScaler* sc, const ParamDesc* desc) {
  ;; // nothing to do
}

s32 scaler_fix_val(io_t in) {
  // io_t is 16-bit signed integer
  return in << 15;
}

void scaler_fix_str(char* dst, io_t in) {
  print_fix16(dst, in << 15);
}

io_t scaler_fix_in(s32 val) {
  return val >> 15;
}
