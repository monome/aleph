//------------------
//--- a basic oscillator
//--- no modulation

#ifndef _OSC_H_
#define _OSC_H_

#include "types.h"

#define NUM_OSCS 4

// set phase directly
extern void osc_set_phase(u16 idx, u32 val);

// set phase increment directly
extern void osc_set_phi(u16 idx, u32 phi);

// set upper 16 bits of phase increment from fract32
extern void osc_set_phi_upper(u16 idx, fract32 val);

// set loewr 16 bits of phase increment from fract32
extern void osc_set_phi_lower(u16 idx, fract32 val);

// set oscillator amplitude
extern void osc_set_amp(u16 idx, fract32 amp);

// set mix point amplitude
extern void osc_set_mix(u16 oscIdx, u16 outIdx, fract32 amp);

// process one block and mix to buffer 
//extern void osc_process_block(fract32* dst, u16 frameCount);
extern void osc_process_block( u16 idx, buffer_t *outChannels);

#endif
