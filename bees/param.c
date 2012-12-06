/* param.c
 * bees
 * aleph
 */


// common
#include "param_common.h"
// bees
#include "ui.h"
#include "net.h"
#include "net_protected.h"
#include "menu.h"
#include "param.h"

#ifdef ARCH_AVR32
#include "bfin.h"
#endif

// get value for param at given idx
io_t get_param_value(u32 idx) {
  // NOTE: io_t is float, change this if it becomes otherwise
  return (io_t)(net->params[idx].data.value.asFloat);
}

// get preset-enabled for param at given idx
u8 get_param_preset(u32 idx) {
  return net->params[idx].preset;
}

// set value for param at given idx
void set_param_value(u32 idx, io_t val) {
  // NOTE: io_t is float, change this if it becomes otherwise
  // don't cast, hopefully we get a warning
  set_param_float_value(idx, val);
}


// set a parameter value using a 32bit integer
// mapped to the parameter's float range
void set_param_step_value(u32 idx, s32 val) {

  //f32 fval;
  net->params[idx].data.value.asFract = val;
  // FIXME:
  // not really using this function right now... 
  /*
  //  net->params[idx].ival = val;
  //////
  // DEBUG
  fval = (f32)val * PARAM_MAX_RF;
  fval += 1.f;
  fval *= 0.5f;
  fval *= (net->params[idx].max - net->params[idx].min);
  fval += net->params[idx].min;
  //////t.p
  net->params[idx].fval = fval;
  param_feedback(idx);
  */
}

// set a parameter value using float
void set_param_float_value(u32 idx, f32 val) {

  /*
  s32 ival;
  f32 fmin, fmax;
  fmin = net->params[idx].min;
  fmax = net->params[idx].max;
  if (val < fmin) { val = fmin; }
  if (val > fmax) { val = fmax; }
  ival = (s32)( (val - fmin) / (fmax - fmin) * PARAM_MAX_F );
  net->params[idx].fval = val;
  net->params[idx].ival = ival;
  param_feedback(idx);
  */
  net->params[idx].data.value.asFloat = val;
  param_feedback(idx);
#ifdef ARCH_AVR32
  bfin_set_param(idx, net->params[idx].data.value.asFloat);
#endif

}

