/*
  crossfaders.
 */

#include "types.h"

// linear crossfade (dumb)
typedef struct _fadeLin  {
  // inputs
  fract32 x; 
  fract32 y;
  // fade amount [0-1]
  fract32 pos;
} fadeLin;

// equal-power crossfade
typedef struct _fadeEP {
  // inputs
  fract32 x; 
  fract32 y;
  // fade amount [0-1]
  fract32 pos;
} fadeEP;


// setting any of the inputs returns the crossfaded value.
extern fract32 fade_lin_x(fadeLin* fade, fract32 v);
extern fract32 fade_lin_y(fadeLin* fade, fract32 v);
extern fract32 fade_lin_pos(fadeLin* fade, fract32 v);
// recalculate without input
extern fract32 fade_lin_calc(fadeLin* fade);
   
// setting any of the inputs returns the crossfaded value.
extern fract32 fade_ep_x(fadeEP* fade, fract32 v);
extern fract32 fade_ep_y(fadeEP* fade, fract32 v);
extern fract32 fade_ep_pos(fadeEP* fade, fract32 v);
// recalculate without input
extern fract32 fade_ep_calc(fadeEP* fade);
