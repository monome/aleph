/* inputs.h
   lppr
   aleph

   scaling/conversion for parameter inputs.

 */

#ifndef _ALEPH_AVR32_APP_LPPR_INPUTS_H_
#define _ALEPH_AVR32_APP_LPPR_INPUTS_H_

#include "fix.h"
#include "types.h"

// maximum input value:  16 bits
/// fixme: 
/// we can always increase the bit-depth...

#define IN_MAX		0xffff
// convert to fractional part of fix16
#define IN_FR16(x)     ( (x) & 0x0000ffff )
// convert to fractional part of fix16
#define FR16_IN(x)      ( (x) & 0x0000ffff )
// convert to fract32
#define IN_FR32(x)      ( (x) << 16 )
// convert from fract32
#define FR32_IN(x)      ( (x) >> 16 )


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
