/* mono.c
 * nullp
 * 
 * a monosynth module using FM chain synthesis (dx-7 style)
 */

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "simple_string.h"

#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

// audio lib
#include "env.h"
#include "filters.h"
#include "module.h"
#include "table.h"
#include "types.h"

/// DEBUG
//static u32 framecount = 0;

//typedef union { u32 u; s32 s; fix16 fix; fract32 fr; } pval;

///// inputs
enum params {
  eParamNumParams
};


//-------------------------
//----- extern vars (initialized here)
moduleData_t * moduleData; // module data

//-------------------
//----------------------
//----- static functions

//----------------------
//----- external functions

void module_init(void) {

}

// de-init
void module_deinit(void) {
}

// set parameter by value
///// FIXME: the represeentation of this value is totally arbitrary!
void module_set_param(u32 idx, pval v) {

}

// get number of parameters
extern u32 module_get_num_params(void) {
  return eParamNumParams;
}

// frame callback

void module_process_frame(void) {

  out0 = in0;
  out1 = in1;
  out2 = in2;
  out3 = in3; 
}

static u8 ledstate = 0;
u8 module_update_leds(void) {
  return ledstate;
}


void module_handle_button(const u16 state) {
 
}

