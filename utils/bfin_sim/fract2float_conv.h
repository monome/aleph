#ifndef _FRACT2FLOAT_CONV_H
#define _FRACT2FLOAT_CONV_H

#include "types.h"

fract32 fr16_to_fr32(fract16 _x);


fract16 fr32_to_fr16(fract32 _x);

fract32 float_to_fr32 (float _x);
fract16 float_to_fr16 (float _x);

float   fr32_to_float (fract32 _x);
float   fr16_to_float (fract16 _x);

#endif /* _FRACT2FLOAT_CONV_H */
