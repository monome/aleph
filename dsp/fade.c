#include <fract_math.h>
#include "fix.h"
#include "table.h"

#include "fade.h"


#define FADE_SINE_BUF_SIZE 1024
#define FADE_SINE_BUF_SIZE_FIX (1024 << 16)

static const fract32 sineTab[FADE_SINE_BUF_SIZE] = {
#include "halfsine_1024.inc"
}  ;


//---------------
//--- linear crossfade

//static fract32 fade_lin_calc(fadeLin* fade);

// setting any of the inputs returns the crossfaded value.
fract32 fade_lin_x(fadeLin* fade, fract32 v) {
  fade->x = v;
  return fade_lin_calc(fade);
}

fract32 fade_lin_y(fadeLin* fade, fract32 v) {
  fade->y = v;
  return fade_lin_calc(fade);
}

fract32 fade_lin_pos(fadeLin* fade, fract32 v) {
  fade->pos = v;
  return fade_lin_calc(fade);
}

// recalculate without input
fract32 fade_lin_calc(fadeLin* fade) {
  fract32 ret = mult_fr1x32x32(fade->x, fade->pos);
  ret = add_fr1x32(ret, mult_fr1x32x32(fade->y, sub_fr1x32(FR32_MAX, fade->pos)) );
  return ret;
}

//---------------
//--- equal-power crossfade
//static fract32 fade_ep_calc(fadeEP* fade);

// setting any of the inputs returns the crossfaded value. 
fract32 fade_ep_x(fadeEP* fade, fract32 v) {
  fade->x = v;
  return fade_ep_calc(fade);
}

fract32 fade_ep_y(fadeEP* fade, fract32 v) {
  fade->y = v;
  return fade_ep_calc(fade);
}

fract32 fade_ep_pos(fadeEP* fade, fract32 v) {
  fade->pos = v;
  return fade_ep_calc(fade);
}

// recalculate without input
fract32 fade_ep_calc(fadeEP* fade) {
  // FIXME: far from optimal.
  /// fractional position converted to table index
  fix16 xIdx = fix16_mul(FRACT_FIX16(fade->pos), FADE_SINE_BUF_SIZE_FIX);
  fix16 yIdx = fix16_sub(FADE_SINE_BUF_SIZE_FIX, xIdx);
  fract32 xmul = table_lookup_idx(sineTab, FADE_SINE_BUF_SIZE, xIdx);
  fract32 ymul = table_lookup_idx(sineTab, FADE_SINE_BUF_SIZE, yIdx);
  return add_fr1x32( mult_fr1x32x32(fade->x, xmul), mult_fr1x32x32(fade->y, ymul) ); 
}
