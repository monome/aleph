// boolean parameter

#include "print_funcs.h"

#include "op_math.h"
#include "scaler_bool.h"

void scaler_bool_init(void* scaler) {
  ParamScaler* sc = (ParamScaler*)scaler;

  sc->inMin = 0;
  sc->inMax = 1;

}

s32 scaler_bool_val(void* scaler, io_t in) {
  return (in > 0);
}

void scaler_bool_str(char* dst, void* scaler, io_t in) {
  print_fix16(dst, in > 0 ? fix16_one : 0); 
}

io_t scaler_bool_in(void* scaler, s32 val) {
  return (val > 0);
}

// increment input by pointer, return value
s32 scaler_bool_inc(void* sc, io_t* pin, io_t inc ) {
  /* *pin = op_sadd(*pin, inc); */
  /* return (*pin > 0); */
  *pin = (inc > 0);
  return *pin;
}
