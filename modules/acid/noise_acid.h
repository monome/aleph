#ifndef _NOISE_DSYN_H_
#define _NOISE_DSYN_H_

#include "types.h"

extern void acid_noise_init(void);
extern void acid_noise_reset(int voice);
extern fract32 acid_noise_next(int voice);

#endif
