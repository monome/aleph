#include <stdlib.h>
#include "echoTap.h"
#include "scrubTap.h"
#include "pan.h"
#include "noise.h"
#include "ricks_tricks.h"

// intialize tap
extern void scrubTap_init(scrubTap* tap, echoTap* echoTap){
  tap->echoTap = echoTap;
  quadraturePhasor_init(&(tap->quadraturePhasor));
  tap->length = 1024;
  tap->frequency = hzToDimensionless (100);
}

extern void scrubTap_next(scrubTap* tap){
  quadraturePhasor_pos_next_dynamic(&(tap->quadraturePhasor), tap->frequency);
}

extern fract32 scrubTap_read_xfade (scrubTap* scrubTap) {

  s32 sinPos = quadraturePhasor_pos_sinRead(&(scrubTap->quadraturePhasor));
  s32 cosPos = quadraturePhasor_pos_cosRead(&(scrubTap->quadraturePhasor));
  fract32 sinHead = echoTap_read_xfade(scrubTap->echoTap,
				       (mult_fr1x32x32(scrubTap->length,
						       sinPos)));
  fract32 cosHead = echoTap_read_xfade(scrubTap->echoTap,
				       (mult_fr1x32x32(scrubTap->length,
						       cosPos)));

  fract32 sinMix = s32_halfWave_env(sinPos);
  fract32 cosMix = s32_halfWave_env(cosPos);
  return  add_fr1x32 (mult_fr1x32x32 (sinHead, sinMix),
		     mult_fr1x32x32 (cosHead, cosMix));
}
