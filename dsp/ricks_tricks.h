#include "types.h"


typedef struct {
  fract32 lastIn;
  fract32 lastOut;
} hpf;

fract32 hpf_init (hpf *myHpf);
fract32 hpf_next_dynamic (hpf *myHpf, fract32 in, fract32 freq);
fract32 lpf (fract32 in, fract32 freq);

fract32 osc (fract32 phase);

#define simple_slew(x, y, slew) x = add_fr1x32( y,		     \
						mult_fr1x32x32(slew,	\
							       sub_fr1x32(x, y)))
fract32 max (fract32 x, fract32 y);
fract32 min (fract32 x, fract32 y);
