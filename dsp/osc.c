#include <fract2float_conv.h>

#include "osc.h"

//----------------
//--- static vars
/// assume all oscs have the same samplerate
/// phase increment at 1hz:
static fix16 ips;
/// phase increment limits
static fix16 incMin;
static fix16 incMax;
static u32 incRange;
// multiplier to map w-> max shape
static u32 shapeLimMul;

//------------------
//---- static functions


// calculate modulated and bandlimited waveshape
void osc_calc_shape(sco* osc) {
  fract32 sm; // mod shape
  fract32 sl; // shape limit given current freq
  sm = add_fr1x32(osc->shape, osc->shapeMod);
  //- hacky magic formula for pseudo-bandlimiting:
  //- with maximal bandlimiting, want to limit shape to a function of unit freq
  //- max freq = min shape, min freq = max shape
  // :
  // map phase increment to [0,1] fract32
  sl  = ((u32)(osc->inc) - incRange) * shapelimMul;
  // invert [0,1] to [1,0]
  sl = (s32)sl * -1 + 0x7fffffff;
  // limit
  if(sl < sm) {
    sm = dsp_lerp32(sm, sl, osc->bandLim);
  }
  osc->shapeMod = sm;
}

// calculate phase incremnet
void osc_calc_inc( osc* osc) {
  filter_1p_lo_in( &(osc->lpInc), fix16_mul(osc->ratio, fix16_mul(osc->hz, ips)) );
}

//----------------
//--- extern funcs

// initialize given table data and samplerate
void osc_init(osc* osc, fract32** tab, u32 sr) {
  osc->tab = tab;
  ips = fix16_from_float( (f32)WAVE_TAB_SIZE / (f32)sr );
  incMin = fix16_mul(ips, OSC_HZ_MIN);
  incMax = fix16_mul(ips, OSC_HZ_MAX);
  incRange = (u32)incMax - (u32)incMin;
  shapeLimMul = 0x7fffffff / incRange;
}

// set waveshape (table)
void osc_set_shape(osc* osc, fract32 shape) {
  osc->shape = shape;
  osc_calc_shape(osc);
}

// set base frequency in hz
void osc_set_hz(osc* osc, fix16 hz) {
  osc->hz = hz;
  osc_calc_inc(osc);
}

// set fine-tuning ratio
void osc_set_tune(osc* osc, fix16 ratio) {
  osc->ratio = ratio;
  osc_calc_inc(osc);
}

// phase modulation amount
void osc_set_pm(osc* osc, fract32 wmAmt);
// shape modulation amount
void osc_set_wm(osc* osc, fract32 wmAmt);

// phase modulation input
void osc_pm_in(osc* osc, fract32 pm) {
  
}

// shape modulation input
void osc_wm_in(osc* osc, fract32 wm) {
  osc->shapeMod = BIT_ABS_32(wm);
  osc_calc_shape(osc);
}


// set bandlimiting
void osc_set_bl(osc* osc, fract32 bl);
// get next value
fract32 osc_next(void);
