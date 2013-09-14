/* inputs.h
   lppr
   aleph

   scaling/conversion for parameter inputs.

 */

#ifndef _ALEPH_AVR32_APP_FLRY_INPUTS_H_
#define _ALEPH_AVR32_APP_FLRY_INPUTS_H_

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

#endif
