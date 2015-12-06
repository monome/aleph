//------------------
//--- a basic oscillator
//--- no modulation

#ifndef _OSC_H_
#define _OSC_H_

#include "types.h"

// size of wavetable
#define WAVE_TAB_SIZE 	1024
#define WAVE_TAB_BITS 	10
#define WAVE_TAB_SIZE_1 ((WAVE_TAB_SIZE) - 1)
// size of rshift to get index from phase
#define WAVE_IDX_SHIFT (32 - WAVE_TAB_BITS)
// mask to get fractional multiplier
#define WAVE_FRACT_MASK ((1 << WAVE_IDX_SHIFT) - 1)
// shift fract multiplier to fract32 range
#define WAVE_FRACT_SHIFT (WAVE_TAB_BITS - 1)


// set phase increment directly
extern void osc_set_phi(u32 phi);

// set upper 16 bits of phase increment from fract32
extern void osc_set_phi_upper(fract32 val);

// set loewr 16 bits of phase increment from fract32
extern void osc_set_phi_lower(fract32 val);


// set amplitudee
extern void osc_set_amp(fract32 amp);

// process one block and mix to buffer 
extern void osc_process_block(fract32* dst);

#endif
