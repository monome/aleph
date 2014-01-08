/* 
   bl_osc.h
   aleph-dsp

   a band-limited oscillator with variable waveshape (sine-saw),
   pulse width modulation (saw-square-pulse)
   and phase modulation.
*/

#include "fix.h"
#include "types.h"

#define WAVE_TAB_SIZE 	2048
#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)
#define WAVE_TAB_MAX16 	(WAVE_TAB_SIZE * FIX16_ONE - 1)

typedef struct _blOsc {
  // table of wavetables
  fract32** wavtab;
  // normalized waveshape
  fract32 shape;
  // current table size
  u32 tabSize;
  // normalized frequency
  fract32 freq;
  // phase as fractional index
  fix16 idx;
  // phase as fractional index, modulated
  fix16 idxMod;
  // index increment
  fix16 inc;
  // inversion amount 
  fract32 invAmp;
  // inversion phase
  fix16 invPhase;
} blOsc;

// set waveshape (table)
extern void blosc_set_shape(blOsc* osc, fract32 shape);
// set frequency
extern void blosc_set_freq(blOsc* osc, fract32 freq);
// set modulated phase
extern void blosc_set_pm(blOsc* osc, fract32 pm);
// set inversion amount
extern void blosc_set_inv(blOsc* osc, fract32 inv);
// set inversion phase
extern void blosc_set_inv_phase(blOsc* osc, fix16 phase);

// get next value
extern fract32 blosc_get_next(blOsc* osc);
