/* inputs.h
   lppr
   aleph

   scaling/conversion for parameter inputs.

 */

#ifndef _ALEPH_AVR32_APP_DSYN_INPUTS_H_
#define _ALEPH_AVR32_APP_DSYN_INPUTS_H_

#include "fix.h"
#include "types.h"

// 24-but input
#define INPUT_MAX 0xfffff

// initialize tables
extern void inputs_init(void);

// get amplitude
extern fract32 input_amp(u32 in);
// get db
extern fix16 input_db(u32 in);

// get note number 
extern fix16 input_note(u32 in);
// get hz
extern fix16 input_hz(u32 in);
// get svf coefficient
extern fract32 input_freq(u32 in);

// convert duration in seconds (float) to slew coefficient (fract32)
extern fract32 sec_to_slew(float sec);

// convert frequency in hz (float) to svf cutoff coefficient (fract32) 
extern fract32 hz_to_svf(float hz);

// convert fr32 to float
extern s32 float_to_fr32(float x);

#endif
