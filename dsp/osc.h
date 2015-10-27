/* osc.h
   dsp
   aleph

   a simple wavetable oscillator with phase and shape modulation.
 */

#ifndef _ALEPH_DSP_OSC_H_
#define _ALEPH_DSP_OSC_H_

#include "filter_1p.h"
#include "fix.h"

// base-frequency limits in fix16
#define OSC_HZ_MIN 0x00010000      // 1 hz
#define OSC_HZ_MAX 0x40000000    // 16384 hz
#define OSC_HZ_RADIX 15

//---- expected parameters for wavetable data
// how many wavetables
#define WAVE_TAB_NUM 5
// size of each table
#define WAVE_TAB_SIZE 	1024
#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)
#define WAVE_TAB_MAX16 	(WAVE_TAB_SIZE * FIX16_ONE - 1)
// rshift from shape variable to get table index
#define WAVE_TAB_RSHIFT 29
// mask to get interpolation constant
#define WAVE_SHAPE_MASK 0x1fffffff
// lshift after mask to get multiplier 
#define WAVE_TAB_LSHIFT 2

// wavetable type: pointer to 2d array
typedef const fract32 (*wavtab_t) [WAVE_TAB_NUM][WAVE_TAB_SIZE];

// class structure
typedef struct _osc {
  // output value
  fract32 val;
  // wavetable data (pointer to table of tables)
  wavtab_t tab;
  // normalized waveshape
  fract32 shape;
  // smoothed and normalized waveshape
  //  fract32 shapeSmooth;
  // modulated and bandlimited waveshape
  fract32 shapeMod;
  // frequency in hz
  fix16 hz;
  // tuning ratio
  fix16 ratio;
  // base phase as fractional index
  fix16 idx;
  // modulated phase
  fix16 idxMod;
  // fixed-point index increment
  fix16 inc;
  // bandlimiting coefficient [0-1]
  fract32 bandLim;
  // phase modulation amount [0-1)
  fract32 pmAmt;
  // shape modulation amount [0-1)
  fract32 wmAmt;

  /// store last modulation input values for recalculation
  fract32 pmIn, wmIn;

  /// 1pole filters for smoothing phase increment, shape, modulation params
  filter_1p_lo lpInc;
  filter_1p_lo lpShape;
  filter_1p_lo lpPm;
  filter_1p_lo lpWm;
  
  //// TODO: can get more shapes, at slight expense, by invert+shift+sum
  // inversion amount 
  //  fract32 invAmp;
  // inversion phase
  //  fix16 invPhase;
} osc;

// initialize given table data and samplerate
extern void osc_init(osc* osc, wavtab_t tab, u32 sr);

// set waveshape (table)
extern void osc_set_shape(osc* osc, fract32 shape);
// set base frequency in hz
extern void osc_set_hz(osc* osc, fix16 hz);
// set fine-tuning ratio
extern void osc_set_tune(osc* osc, fix16 ratio);

// phase modulation amount
extern void osc_set_pm(osc* osc, fract32 wmAmt);
// shape modulation amount
extern void osc_set_wm(osc* osc, fract32 wmAmt);

// phase modulation input
extern void osc_pm_in(osc* osc, fract32 pm);
// shape modulation input
extern void osc_wm_in(osc* osc, fract32 wm);

// set bandlimiting coefficient
extern void osc_set_bl(osc* osc, fract32 bl);

// compute next value
extern fract32 osc_next( osc* osc);

#endif
