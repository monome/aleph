//------------------
//--- a basic oscillator
//--- no modulation

#ifndef _OSC_H_
#define _OSC_H_

#include "types.h"

// set phase increment directly
extern void osc_set_phi(u32 phi);

// set upper 16 bits of phase increment from fract32
extern void osc_set_phi_upper(fract32 val);

// set loewr 16 bits of phase increment from fract32
extern void osc_set_phi_lower(fract32 val);

// set amplitudee
extern void osc_set_amp(fract32 amp);

// process one block and mix to buffer 
//extern void osc_process_block(fract32* dst, u16 frameCount);
extern void osc_process_block(buffer_t *outChannels,
			      const u8 numChannels, const u8 numFrames);

#endif
