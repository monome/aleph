/* scaler.c
   bees

   parameter-scaling module.

*/


#include "scaler.h"
#include "types.h"

// type-specific includes
#include "scalers/scaler_amp.h"

//-- function types
// initialize scaler for given param
typedef void (*scaler_init_fn)(scaler* sc, const ParamDesc* desc);
// get DSP value for given type, input
typedef s32 (*scaler_get_value_fn)(io_t in);
// get readable value for given type, input
// this datatype can change! 
// users should check the associated paramDesc if needed
typedef s32 (*scaler_get_rep_fn)(scaler* sc, io_t in);
// perform a tuning routine
typedef s32 (*scaler_tune_fn)(scaler* sc, u8 tuneId, io_t in);

// class structure
struct _scaler { 
   // get value
  scaler_get_value_fn get_value;
  // get ui representation
  scaler_get_rep_fn get_rep;
  // array of tuning functions
  scaler_tune_fn * tune;
  // num tuning functions
  u8 numTune;
};

// initialization flags
static u8 initFlags[eParamNumTypes] = {0};


// array of pointers to initialization functions.
scaler_init_fn scaler_init_pr[eParamNumTypes] = {
  
};

//-------------------------------,
//---- extern function
void scaler_init(scaler* sc, const ParamDesc* desc) {
  if(initFlags[desc->type]) return;
  (*(scaler_init_pr[desc->type]))(sc, desc);
  initFlags[desc->type] = 1;
}
