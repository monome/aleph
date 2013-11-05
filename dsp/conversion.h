#ifndef _CONVERSION_H_
#define _CONVERSION_H_

#include "types.h"
#include "fix.h"
#include "fix32.h"

// seconds in 16.16 to frames in 32.32
extern void sec_to_frames_fract(fix16* time, fix32* samps);

// seconds to frames, truncated
extern u32 sec_to_frames_trunc(fix16 sec);

#endif // _CONVERSION_H_
