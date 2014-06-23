/* higher-quality 32b audio noise

   start with farily uniform distribution
   from XOR-shift method followed by 32b MLCG
   (see numerical recipies, 3rd ed, p.355)

   mix some gaussian for low frequencies

   finish with 2p highpass to remove sub-audible components
*/

#include <fract_math.h>

#include "dsyn.h"
#include "noise_dsyn.h"

static fract32 x[DRUMSYN_NVOICES];
static const u8 b[3] =  { 13, 17, 5 };
static const u32 m = 1597334677;


// rsfhift for "flat" component
static const u8 fs = 2;

// rshift for "gaussian" component
// this will scale frequency of LF components
static const u8 gs = 3;

// seed shouldn't matter much as long as its nonzero... (?)
static const fract32 s[DRUMSYN_NVOICES] = {1, 2, 3, 4};

extern void dsyn_noise_init(void) {
  dsyn_noise_reset(0);
  dsyn_noise_reset(1);
  dsyn_noise_reset(2);
  dsyn_noise_reset(3);
}

extern void dsyn_noise_reset(int voice) { 
  x[voice] = s[voice];
}


extern fract32 dsyn_noise_next(int voice) {
#if 1
  u32 y = (u32) (x[voice]);

  y ^= (y >> b[0]);
  y ^= (y << b[1]);
  y ^= (y >> b[2]);
  y *= m;

  
  x[voice] = (fract32)y;

  return shr_fr1x32((fract32)(x[voice]), 2);

#else
  fract32 z = (x[voice]);
  u32 y ;

  z ^= (z >> b[0]);
  z ^= (z << b[1]);
  z ^= (z >> b[2]);
  z *= m;

  y = (u32)z;

  y ^= (y >> b[0]);
  y ^= (y << b[1]);
  y ^= (y >> b[2]);
  y *= m;

  
  x[voice] = add_fr1x32(
			shr_fr1x32( (fract32)y, fs),
			shr_fr1x32( add_fr1x32(x[voice],
					       shr_fr1x32(z, gs)
					       ),
				    2
				    )
			
			);



  return shr_fr1x32((fract32)(x[voice]), 2);
#endif
}
