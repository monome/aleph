#ifndef _ALEPH_WAVES_SLEW_H_
#define _ALEPH_WAVES_SLEW_H_

/// this is a hack...
/// descriptor utility ends up including this, which is dumb.
#if ARCH_BFIN==1 
#include <fract_math.h>

typedef struct {
  fract32 x;
  fract32 y;
  fract32 c;
} Slew32;


typedef struct {
  fract16 x;
  fract16 y;
  fract16 c;
  fract16 pad;
} Slew16;

#define slew_init( sl, xa, ya, ca)			\
  (sl.x) = (xa);					\
  (sl.y) = (ya);					\
  (sl.c) = (ca);

#define slew32_calc( sl )						\
  (sl).y = add_fr1x32( (sl).x,						\
		       mult_fr1x32x32((sl).c,				\
				      sub_fr1x32((sl).y, (sl).x)	\
				      ));				

#define slew32_sync( sl ) ( sl.x == sl.y )

#define slew16_calc( sl )					\
  (sl).y = add_fr1x16( (sl).x,					\
		       mult_fr1x16((sl).c,			\
				   sub_fr1x16((sl).y, (sl).x)	\
				   ));				

#define slew16_sync( sl ) ( (sl).x == (sl).y )
      
#endif // blackfin architecture

#endif // h guard
