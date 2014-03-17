/* osc.h
   dsp
   aleph

   a simple wavetable oscillator with phase and shape modulation.
 */

#ifndef _ALEPH_DSP_OSC_H_
#define _ALEPH_DSP_OSC_H_

//#include "filter_1p.h"
#include "fix.h"
#include "slew.h"

//---- expected parameters for wavetable data
// how many wavetables
#define WAVE_SHAPE_NUM 5
#define WAVE_SHAPE_BITS 3

// size of each table
#define WAVE_TAB_SIZE 	1024
#define WAVE_TAB_BITS 	10
#define WAVE_TAB_SIZE_1 ((WAVE_TAB_SIZE) - 1)
//#define WAVE_TAB_MAX16 	((WAVE_TAB_SIZE) * FIX16_ONE - 1)

// rshift from shape variable (fract16) to get table index
#define WAVE_SHAPE_IDX_SHIFT 16 - (WAVE_SHAPE_BITS)
// mask to get interpolation multiplier
#define WAVE_SHAPE_MASK (1 << (WAVE_SHAPE_IDX_SHIFT)) - 1
// lshift after mask to get fract16 multiplier 
#define WAVE_SHAPE_MUL_SHIFT 2
// oscillator phase is normalized fract32 in [0, 1)
// rshift to get table index (e.g., top 10 bits)
#define WAVE_IDX_SHIFT 21
// mask to get interpolation multiplier 
#define WAVE_IDX_MASK (1 << (WAVE_IDX_SHIFT)) - 1
// rshift multiplier to fract16 
#define WAVE_IDX_MUL_SHIFT (WAVE_IDX_SHIFT) - 15

// normalized IPS for unipolar 1.31 phase, assuming 48k sampling rate
#define WAVE_IPS_NORM 0xae3c


// wavetable type: pointer to 2d array
typedef const fract32 (*wavtab_t) [WAVE_SHAPE_NUM][WAVE_TAB_SIZE];

// class structure
typedef struct _osc {
  // output valuec
  fract32 val;
  // wavetable data (pointer to table of tables)
  wavtab_t tab;
  // frequency in hz
  fix16 hz;
  // tuning ratio
  fix16 ratio;

  // base phase as fractional index
  //  fix16 idx;
  // modulated phase
  //  fix16 idxMod;

  // base phase, normalized fract32 in [0, 1)
  fract32 phase;
  // modulated phase
  fract32 phaseMod;

  // fixed-point index increment
  fix16 inc;
  // bandlimiting coefficient [0-1]
  //  fract16 bandLim;
  // phase modulation amount [0-1)
  fract16 pmAmt;
  // shape modulation amount [0-1)
  fract16 wmAmt;

  // normalized waveshape
  fract16 shape;
  // modulated waveshape
  fract16 shapeMod;

  /// store last modulation input values for recalculation
  fract32 pmIn;
  fract32 wmIn;

  /// 1pole filters for smoothing phase increment, shape, modulation params
  Slew16 pmSlew;
  Slew16 wmSlew;
  Slew16 shapeSlew;

  Slew32 incSlew;
   
  //// TODO: can get more shapes, at slight expense, by invert+shift+sum
  // inversion amount 
  //  fract32 invAmp;
  // inversion phase
  //  fix16 invPhase;
} osc;

// initialize given table data and samplerate
extern void osc_init(osc* osc, wavtab_t tab, u32 sr);

// set waveshape (table)
extern void osc_set_shape(osc* osc, fract16 shape);
// set base frequency in hz
extern void osc_set_hz(osc* osc, fix16 hz);
// set fine-tuning ratio
extern void osc_set_tune(osc* osc, fix16 ratio);

// phase modulation amount
extern void osc_set_pm(osc* osc, fract16 wmAmt);
// shape modulation amount
extern void osc_set_wm(osc* osc, fract16 wmAmt);

// phase modulation input
extern void osc_pm_in(osc* osc, fract32 pm);
// shape modulation input
extern void osc_wm_in(osc* osc, fract32 wm);

// set bandlimiting coefficient
// extern void osc_set_bl(osc* osc, fract32 bl);

// compute next value
extern fract32 osc_next( osc* osc);

#endif
