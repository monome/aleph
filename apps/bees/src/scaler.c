/* scaler.c
   bees

   parameter-scaling module.

*/


#include "scaler.h"
#include "types.h"

// type-specific
#include "scalers/scaler_amp.h"


// array of poitners to initialization functions.
void(*scaler_init_pr[])(fix16 min, fix16 max) = {
};

// array of pointers to scaling functions.
s32 (*scaler_val_pr[])(io_t in) = {
  
};

// array of pointers to representation functions
fix16 (*scaler_rep_pr[])(io_t in) = {
};

// initialization flags
static u8 initFlags[eParamNumTypes] = {0};

//-------------------------------,
//---- extern function
void scaler_init(ParamType type, fix16 min, fix16 max) {
  if(initFlags[type]) return;
  (*(scaler_init_pr[type]))(min, max);
}
