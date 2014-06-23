#ifndef _NOISE_DSYN_H_
#define _NOISE_DSYN_H_

#include "types.h"

extern void dsyn_noise_init(void);
extern void dsyn_noise_reset(int voice);
extern fract32 dsyn_noise_next(int voice);

#endif
