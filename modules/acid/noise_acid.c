/* higher-quality 32b audio noise

   start with farily uniform distribution
   from XOR-shift method followed by 32b MLCG
   (see numerical recipes, 3rd ed, p.355)

*/

#include <fract_math.h>

#include "acid.h"
#include "noise_acid.h"

extern void acid_noise_init(fract32 *x) {
  *x = 1;
}

extern fract32 acid_noise_next(fract32 *x) {
  u32 y = (u32) *x;
  y ^= (y >> 13);
  y ^= (y << 17);
  y ^= (y >> 5);
  y *= 1597334677;
  *x = y;
  return y;
}
