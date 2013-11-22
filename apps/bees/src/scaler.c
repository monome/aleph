/* scaler.c
   bees

   parameter-scaling module.

*/


#include "scaler.h"
#include "types.h"

// data type
typedef struct _paramScaler {
  
  s32* data;
  
} ParamScaler;

// array of scaling functions.
s32(*scaler_val_pr[])(io_t in) = {
};

// array of representation functions
s32(*scaler_rep_pr[])(io_t in) = {
};

// array of initialization functions.
void(*scaler_init_pr[])(void) = {
};

// initialization flags
static u8 initFlags[eParamNumTypes] = {0};

//-------------------------------,
//---- extern function
void scaler_init(ParamType type) {
  if(initFlags[type]) return;

  (*(scaler_init_pr[type]))();
}
