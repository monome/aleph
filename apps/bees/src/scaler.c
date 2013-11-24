/* scaler.c
   bees

   parameter-scaling module.

*/


#include "scaler.h"
#include "types.h"

// type-specific includes
#include "scalers/scaler_amp.h"

// array of pointers to initialization functions.
scaler_init_fn scaler_init_pr[eParamNumTypes] = {
  &scaler_amp_init,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

//-------------------------------,
//---- extern function
void scaler_init(scaler* sc, const ParamDesc* desc) {
  (*(scaler_init_pr[desc->type]))(sc, desc);
}

s32 scaler_get_value(scaler* sc, io_t in) {
  return sc->get_val(in);
}

s32 scaler_get_rep(scaler* sc, io_t in) {
  return sc->get_rep(in);
}
