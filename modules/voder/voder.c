/* voder.c
   aleph-bfin

   Software Emulation of the Bell Labs Voder
*/

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "types.h"

// aleph-bfin
#include "bfin_core.h"
#include "cv.h"
#include "gpio.h"
#include "fract_math.h"

// audio
#include "filter_1p.h"
#include "biquad.h"
#include "filter_crossover.h"
#include "noise.h"
#include "ricks_tricks.h"
#include "osc_polyblep.h"

#include "module.h"

/// custom
#include "params.h"

// data structure of external memory
typedef struct _voderData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} voderData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
voderData* pVoderData;

// dac values (u16, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;

//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}

/* biquad my_bq[4]; */

phasor vocalOsc;

crossover_8_band carrier_8_band_xo;
fract32 bandVolumes[8];

fract32 plosiveKick;
fract32 plosiveDecay;
fract32 vowelLevel;
fract32 hissLevel;
fix16 vowelHz;
fix16 vowelTune;
lcprng hissSource;

crossover_8_band formant_8_band_xo;
trackingEnvelopeLog formantEnvs[8];


void module_init(void) {
  // init module/param descriptor
  pVoderData = (voderData*)SDRAM_ADDRESS;

  gModuleData = &(pVoderData->super);
  strcpy(gModuleData->name, "voder");

  gModuleData->paramData = (ParamData*)pVoderData->mParamData;
  gModuleData->numParams = eParamNumParams;

  filter_1p_lo_init( &(cvSlew[0]), 0 );
  filter_1p_lo_init( &(cvSlew[1]), 0 );
  filter_1p_lo_init( &(cvSlew[2]), 0 );
  filter_1p_lo_init( &(cvSlew[3]), 0 );

  param_setup( eParam_cvSlew0, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew1, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew2, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew3, PARAM_CV_SLEW_DEFAULT );

  param_setup( eParam_cvVal0, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal1, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal2, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal3, PARAM_CV_VAL_DEFAULT );

  param_setup( eParam_band0Vol, PARAM_AMP_0 >> 2 );
  param_setup( eParam_band1Vol, PARAM_AMP_0 >> 2 );
  param_setup( eParam_band2Vol, PARAM_AMP_0 >> 2 );
  param_setup( eParam_band3Vol, PARAM_AMP_0 >> 2 );
  param_setup( eParam_band4Vol, PARAM_AMP_0 >> 2 );
  param_setup( eParam_band5Vol, PARAM_AMP_0 >> 2 );
  param_setup( eParam_band6Vol, PARAM_AMP_0 >> 2 );
  param_setup( eParam_band7Vol, PARAM_AMP_0 >> 2 );

  //these params will control plosives & hiss
  param_setup (eParam_plosiveKick, 0);
  param_setup (eParam_plosiveDecay, 0x01F40000);

  param_setup( eParam_hissLevel, 0);
  param_setup( eParam_vowelLevel, PARAM_AMP_0 >> 2);

  param_setup( eParam_vowelHz, 220 << 16 );
  param_setup( eParam_vowelTune, FIX16_ONE );


  /* set_biquad_lpf(&my_bq, (FR32_MAX/48), float_c8x24(0.5)); */

  //hpf
  /* set_biquad_coeffs_from_floats(&my_bq[0],-1.7990948292036408,0.8175108069275849,0.9041514090328064,-1.8083028180656129,0.9041514090328064); */
  /* set_biquad_coeffs_from_floats(&my_bq[1],-1.7990948292036408,0.8175108069275849,0.9041514090328064,-1.8083028180656129,0.9041514090328064); */

  //lpf
  /* set_biquad_coeffs_from_floats(&my_bq[2],-1.7990948292036408,0.8175108069275849,0.004603994430986024,0.009207988861972048,0.004603994430986024); */
  /* set_biquad_coeffs_from_floats(&my_bq[3],-1.7990948292036408,0.8175108069275849,0.004603994430986024,0.009207988861972048,0.004603994430986024); */
  /* set_biquad_coeffs_from_floats(&my_bq, -1.9384594117996916, 0.954866975559162, */
  /* 				0.0631862331411181, 0.0, -0.0631862331411181); */
  /* my_bq.a1 = float_c8x24(-1.9384594117996916); */
  /* my_bq.a2 = float_c8x24(0.954866975559162); */
  /* my_bq.b0 = float_c8x24(0.0631862331411181); */
  /* my_bq.b1 = float_c8x24(0.0); */
  /* my_bq.b2 = float_c8x24(-0.0631862331411181); */

  /* printf("%d * %d = %d\n", FR8_24_UNITY, FR8_24_UNITY, */
  /* 	 mult_fr8x24(FR8_24_UNITY, FR8_24_UNITY)); */
  /* fract32 unity = float_to_radix32(1.0,7); */
  /* printf("%d * %d = %d\n", float_to_radix32(0.1, 7), float_to_radix32(10.0,7), */
  /* 	 mult_fr8x24(float_to_radix32(10.0,7), float_to_radix32(0.1, 7))); */
  crossover_8_band_init(&carrier_8_band_xo);
  crossover_8_band_init(&formant_8_band_xo);

  phasor_init (&vocalOsc);
  /* vocalOsc.freq = FR32_MAX / 48; */
  lcprng_reset(&hissSource, 1);
  int i;
  for(i = 0; i < 8; i++) {
    trackingEnvelopeLog_init(&(formantEnvs[i]));
    formantEnvs[i].up = SLEW_1MS;
    formantEnvs[i].down = SLEW_10MS;
  }
#ifndef ARCH_BFIN
  printf("%d x %d = %d\n", 0x10000, 0x10000, fix16_mul(0x10000, 0x10000));
  printf("%d x %d = %d\n", FR32_MAX, FR32_MAX, fix16_mul(FR32_MAX, FR32_MAX));

  fix16 one_half = float_to_radix32(1.0, 7);
  /* printf("0.5*0.5=%d\n", (one_half * one_half) >> 7); */
  printf("%d^2=%d\n", one_half, mult_fr7_24(one_half, one_half));

  fract32 i;
  FILE *foo;
  foo = fopen("foo", "w");
  for (i=FR32_MIN;i<FR32_MAX;i = add_fr1x32(i,FR32_MAX / 1000)) {
    fprintf (foo,"%d %d %d\n",
	     polyblep(i, FR32_MAX / 24),
	     saw_polyblep(i, FR32_MAX / 24),
	     square_polyblep(i, FR32_MAX / 24));
  }
  fclose(foo);
  printf("done fettling foo\n");
#endif
}


// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}

//// FIXME:
/* for now, stagger DAC channels across consecutive audio frames
   better method might be:
   - enable DMA tx interrupt
   - each ISR calls the next channel to be loaded
   - last thing audio ISR does is call the first DAC channel to be loaded
   - dac_update writes to 4x16 volatile buffer
*/

void module_process_frame(void) {

  // Update one of the CV outputs
  if(filter_1p_sync(&(cvSlew[cvChan]))) { ;; } else {
    cvVal[cvChan] = filter_1p_lo_next(&(cvSlew[cvChan]));
    cv_update(cvChan, cvVal[cvChan]);
  }

  // Queue up the next CV output for processing next audio frame
  if(++cvChan == 4) {
    cvChan = 0;
  }
  int i;
  
  /* fract32 hp_output, lp_output; */
  /* hp_output = biquad_next(&my_bq[1], biquad_next(&my_bq[0], in[0]));  */
  /* lp_output = biquad_next(&my_bq[3], biquad_next(&my_bq[2], in[0])); */
  /* out[0] = lp_output; */
  /* out[1] = hp_output; */
  /* out[2] = add_fr1x32(hp_output, lp_output); */
  out[0]=out[1]=out[2]=out[3]=0;

  fract32 carrier, buzz, consonant, hiss, plosive;

  buzz = mult_fr1x32x32(shl_fr1x32(saw_polyblep(phasor_next(&vocalOsc), vocalOsc.freq)
				   ,15),
			vowelLevel);
  plosiveKick = mult_fr1x32x32(plosiveKick, FR32_MAX - plosiveDecay);
  hiss = lcprng_next (&hissSource);
  plosive = abs_fr1x32(mult_fr1x32x32(plosiveKick, hiss));
  consonant = add_fr1x32(plosive, mult_fr1x32x32(hissLevel, hiss));

  carrier = add_fr1x32(consonant, buzz);
  carrier = shr_fr1x32(carrier, 2);
  carrier = add_fr1x32(in[0], carrier);
  
  /* crossover_8_band_next(&carrier_8_band_xo, in[0]); */
  crossover_8_band_next(&carrier_8_band_xo, shr_fr1x32(carrier, 2));
  crossover_8_band_next(&formant_8_band_xo, shr_fr1x32(in[1], 4));

  for(i=0; i < 8; i++) {
    bandVolumes[i] = shl_fr1x32(trackingEnvelopeLog_next (&(formantEnvs[i]),
    							  shl_fr1x32(formant_8_band_xo.bands[i],
								     4)),
    				3);
    out[0] = add_fr1x32(out[0],
			mult_fr1x32x32(carrier_8_band_xo.bands[i], bandVolumes[i]));
  }
  out[0] = shl_fr1x32(out[0], 4);    
  out[1] = out[0];
  out[2] = carrier_8_band_xo.bands[5];
  out[3] = carrier_8_band_xo.bands[6];
}

// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE; // which one it this?
  switch(idx) {

  case eParam_cvVal0 :
    filter_1p_lo_in(&(cvSlew[0]), v);
    break;
  case eParam_cvVal1 :
    filter_1p_lo_in(&(cvSlew[1]), v);
    break;
  case eParam_cvVal2 :
    filter_1p_lo_in(&(cvSlew[2]), v);
    break;
  case eParam_cvVal3 :
    filter_1p_lo_in(&(cvSlew[3]), v);
    break;

  case eParam_cvSlew0 :
    filter_1p_lo_set_slew(&(cvSlew[0]), v);
    break;
  case eParam_cvSlew1 :
    filter_1p_lo_set_slew(&(cvSlew[1]), v);
    break;
  case eParam_cvSlew2 :
    filter_1p_lo_set_slew(&(cvSlew[2]), v);
    break;
  case eParam_cvSlew3 :
    filter_1p_lo_set_slew(&(cvSlew[3]), v);
    break;

  case eParam_band0Vol :
    bandVolumes[0] = v;
    break;
  case eParam_band1Vol :
    bandVolumes[1] = v;
    break;
  case eParam_band2Vol :
    bandVolumes[2] = v;
    break;
  case eParam_band3Vol :
    bandVolumes[3] = v;
    break;
  case eParam_band4Vol :
    bandVolumes[4] = v;
    break;
  case eParam_band5Vol :
    bandVolumes[5] = v;
    break;
  case eParam_band6Vol :
    bandVolumes[6] = v;
    break;
  case eParam_band7Vol :
    bandVolumes[7] = v;
    break;

  case eParam_plosiveKick :
    plosiveKick = FR32_MAX >> 1;;
    break;
  case eParam_plosiveDecay :
    plosiveDecay = v;
    break;
  case eParam_hissLevel :
    hissLevel = v;
    break;
  case eParam_vowelLevel :
    vowelLevel = v;
    break;
  case eParam_vowelHz :
    vowelHz = v;
    vocalOsc.freq = fix16_mul(vowelTune, vowelHz);
    break;
  case eParam_vowelTune :
    vowelTune = v;
    vocalOsc.freq = fix16_mul(vowelTune, vowelHz);
    break;
  default:
    break;

  }
}
