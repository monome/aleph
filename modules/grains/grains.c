/* dacs.c
   aleph-bfin

   pitchshift module with 2 'grains'

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

//#include "dac.h"

// audio
#include "filter_1p.h"
#include "module.h"
#include "grain.h"
#include "ricks_tricks.h"

/// custom
#include "params.h"


// total SDRAM is 64M
// each line 16 bit address
#define LINES_BUF_FRAMES 0x10000
//#define LINES_BUF_FRAMES 0xFFFFFF
// try...
//#define LINES_BUF_FRAMES 0x600000
//#define LINES_BUF_FRAMES 0x1000000
//#define LINES_BUF_FRAMES 0xbb8000 // 256 seconds @ 48k
#define PARAM_SECONDS_MAX 0x003c0000
#define AUX_DEFAULT PARAM_AMP_0
#define PAN_DEFAULT PAN_MAX/2
#define FADER_DEFAULT PARAM_AMP_0
#define EFFECT_DEFAULT PARAM_AMP_0

//ADC mix params
ParamValue faderI[4];
ParamValue faderITarget[4];

ParamValue panI[4];
ParamValue panITarget[4];

ParamValue aux1I[4];
ParamValue aux1ITarget[4];

ParamValue aux2I[4];
ParamValue aux2ITarget[4];

ParamValue effectI[4];
ParamValue effectITarget[4];

#define NGRAINS 2
grain grains[NGRAINS];
//Grain mix params
ParamValue sourceMixer3;
ParamValue sourceMixer4;

ParamValue sourceG[NGRAINS];

ParamValue faderG[NGRAINS];
ParamValue faderGTarget[NGRAINS];

ParamValue panG[NGRAINS];
ParamValue panGTarget[NGRAINS];

ParamValue aux1G[NGRAINS];
ParamValue aux1GTarget[NGRAINS];

ParamValue aux2G[NGRAINS];
ParamValue aux2GTarget[NGRAINS];

ParamValue effectG[NGRAINS];
ParamValue effectGTarget[NGRAINS];

ParamValue FM_sourceG[NGRAINS];
ParamValue FM_faderG[NGRAINS];

ParamValue AM_faderG[NGRAINS];
ParamValue AM_sourceG[NGRAINS];
hpf AM_hpf[NGRAINS];
hpf effect_hpf;
lpf grain_lpf[NGRAINS];
fract32 grain_lpf_freq[NGRAINS];

ParamValue phaseG[NGRAINS];

phasor LFO;
fract32 LFO_bus;
fract32 LFO_shape;

fract32 noiseBurstEnv;
fract32 noiseBurstDecay;
lcprng noiseBurstSource;

fract32 CV_gen1Target;
fract32 CV_gen2Target;
fract32 CV_gen1;
fract32 CV_gen2;

// data structure of external memory
typedef struct _grainsData {
  ModuleData super;
  //ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
  volatile fract32 audioBuffer[NGRAINS][LINES_BUF_FRAMES];
} grainsData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;
//-----------------------
//------ static variables

// pointer to all external memory
grainsData* pGrainsData;

// dac values (u16, but use fract32 and audio integrators)

//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v) ;
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}
void module_init(void) {

  // init module/param descriptor
  pGrainsData = (grainsData*)SDRAM_ADDRESS;

  gModuleData = &(pGrainsData->super);
  strcpy(gModuleData->name, "grains");

  gModuleData->paramData = (ParamData*)pGrainsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  param_setup( 	eParam_fader_i1,	FADER_DEFAULT );
  param_setup( 	eParam_pan_i1,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_i1,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_i1,		AUX_DEFAULT );
  param_setup( 	eParam_effect_i1,	EFFECT_DEFAULT );

  param_setup( 	eParam_fader_i2,	FADER_DEFAULT );
  param_setup( 	eParam_pan_i2,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_i2,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_i2,		AUX_DEFAULT );
  param_setup( 	eParam_effect_i2,	EFFECT_DEFAULT );

  param_setup( 	eParam_source_i3,	3 << 16);
  param_setup( 	eParam_fader_i3,	FADER_DEFAULT );
  param_setup( 	eParam_pan_i3,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_i3,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_i3,		AUX_DEFAULT );
  param_setup( 	eParam_effect_i3,	0 );

  param_setup( 	eParam_source_i4,	4 << 16);
  param_setup( 	eParam_fader_i4,	FADER_DEFAULT );
  param_setup( 	eParam_pan_i4,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_i4,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_i4,		AUX_DEFAULT );
  param_setup( 	eParam_effect_i4,	0 );

  //initialise effect bus feedback DC block filter
  hpf_init(&effect_hpf);
  int i;
  //initialise grains
  for (i=0;i<NGRAINS; i++) {
    grain_init(&(grains[i]), pGrainsData->audioBuffer[i], LINES_BUF_FRAMES);
    hpf_init(&(AM_hpf[i]));
    lpf_init(&(grain_lpf[i]));
    grain_lpf_freq[i] = hzToDimensionless(4000);
  }
  
  param_setup( 	eParam_source_g1,	0);
  param_setup( 	eParam_fader_g1,	FADER_DEFAULT );
  param_setup( 	eParam_pan_g1,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_g1,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_g1,		AUX_DEFAULT );
  param_setup( 	eParam_effect_g1,	0 );
  param_setup( 	eParam_phase_g1,	65536);
  param_setup( 	eParam_lpf_g1,          4000 << 16);

  //grain mod params
  param_setup (eParam_FM_source_g1, 0);
  param_setup (eParam_FM_level_g1, 0);
  param_setup (eParam_AM_source_g1, 0);
  param_setup (eParam_AM_level_g1, 0);

  //grain scrubber params
  param_setup (eParam_scrubEnable_g1, 1 << 16);
  param_setup (eParam_scrubPitch_g1, 2 << 16);
  param_setup (eParam_scrubLength_g1, 1000 << 16);
  param_setup (eParam_scrubPitchDetection_g1, 1 << 16);

  //grain echo params
  param_setup(eParam_echoTime_g1, 65536 * 15);
  param_setup(eParam_echoSpeed_g1, 256 * 256 * 1);
  param_setup (eParam_echoEdgeBehaviour_g1, EDGE_WRAP * 65536);
  param_setup (eParam_echoFadeLength_g1, 0);
  param_setup (eParam_echoMin_g1, 0);
  param_setup (eParam_echoMax_g1, 65536 * 1000);

  param_setup (eParam_writeEnable_g1, 1 * 65536);

  //PitchTrack oscillator params
  param_setup (eParam_envAttack_g1, 0x00320000);
  param_setup (eParam_trackingEnv_g1, 1 * 65536);
  param_setup (eParam_trackingPitch_g1, 1 * 65536);
  
  param_setup( 	eParam_source_g2,	0);
  param_setup( 	eParam_fader_g2,	FADER_DEFAULT );
  param_setup( 	eParam_pan_g2,		PAN_DEFAULT );
  param_setup( 	eParam_aux1_g2,		AUX_DEFAULT );
  param_setup( 	eParam_aux2_g2,		AUX_DEFAULT );
  param_setup( 	eParam_effect_g2,	0 );
  param_setup( 	eParam_phase_g2,	65536);
  param_setup( 	eParam_lpf_g2,          4000 << 16);

  //grain mod params
  param_setup (eParam_FM_source_g2, 0);
  param_setup (eParam_FM_level_g2, 0);
  param_setup (eParam_AM_source_g2, 0);
  param_setup (eParam_AM_level_g2, 0);

  //grain scrubber params
  param_setup (eParam_scrubEnable_g2, 1 << 16);
  param_setup (eParam_scrubPitch_g2, 3 << 15);
  param_setup (eParam_scrubLength_g2, 1000 << 16 );
  param_setup (eParam_scrubPitchDetection_g2, 1 << 16);

  //grain echo params
  param_setup(eParam_echoTime_g2, 65536 * 15);
  param_setup(eParam_echoSpeed_g2, 256 * 256 * 1);
  param_setup (eParam_echoEdgeBehaviour_g2, EDGE_WRAP * 65536);
  param_setup (eParam_echoFadeLength_g2, 0);
  param_setup (eParam_echoMin_g2, 0);
  param_setup (eParam_echoMax_g2, 65536 * 1000);

  param_setup (eParam_writeEnable_g2, 1 * 65536);

  //PitchTrack oscillator params
  param_setup (eParam_envAttack_g2, 0x00320000);
  param_setup (eParam_trackingEnv_g2, 1 * 65536);
  param_setup (eParam_trackingPitch_g2, 1 * 65536);

  param_setup (eParam_LFO_speed, 0x00640000);
  param_setup (eParam_LFO_shape, PAN_DEFAULT);
  phasor_init(&LFO);

  param_setup (eParam_cvPatch1, 0);
  param_setup (eParam_cvPatch2, 0);
  param_setup (eParam_cvPatch3, 0);
  param_setup (eParam_cvPatch4, 0);

  param_setup (eParam_noiseBurst, 0);
  param_setup (eParam_noiseBurstDecay, 0x01F40000);
  lcprng_reset(&noiseBurstSource, 1);

  param_setup (eParam_CV_gen1, 0);
  param_setup (eParam_CV_gen2, 0);
}

// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}



/* #define fader_slew(x, y) simple_slew(x, y, SLEW_100MS) */
#define fader_slew(x, y) x = y;
#define grain_slew(x, y) simple_slew(x, y, SLEW_100MS)
/* #define grain_slew(x, y) x = y; */


#define simple_busmix(x, y, fact) x = add_fr1x32(x, mult_fr1x32x32(y, fact))

fract32 effectBus;
fract32 effectBusFeedback;
fract32 grainOutFeedback[NGRAINS];


// dac values (u16, but use fract32 and audio integrators)
static u8 cvPatch[4];
static u8 cvChan = 0;

fract32 patchMatrix[100];

static inline fract32 selectGrainInput(s32 i) {
  return patchMatrix[i];
}

void process_cv (void) {
  cv_update(cvChan, selectGrainInput(cvPatch[cvChan]));

  // Queue up the next CV output for processing next audio frame
  if(++cvChan == 4) {
    cvChan = 0;
  }
  /* cvChan = (cvChan + 1) & 3 */
}

void module_process_frame(void) {

  u8 i;
  //IIR slew
  for (i=0;i<4;i++) {
    fader_slew(faderI[i], faderITarget[i]);
    fader_slew(aux1I[i], aux1ITarget[i]);
    fader_slew(aux2I[i], aux2ITarget[i]);
    fader_slew(panI[i], panITarget[i]);
    fader_slew(effectI[i],effectITarget[i]);
    patchMatrix[i+1] = in[i];
  }
  for (i=0;i<NGRAINS;i++) {
    grain_slew(faderG[i], faderGTarget[i]);
    grain_slew(aux1G[i], aux1GTarget[i]);
    grain_slew(aux2G[i], aux2GTarget[i]);
    grain_slew(panG[i], panGTarget[i]);
    grain_slew(effectG[i],effectGTarget[i]);
  }
  
  //define delay input & output
  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  effectBus = hpf_next_dynamic(&effect_hpf,
			       effectBusFeedback,
			       hzToDimensionless(50));
  for (i=0;i<2;i++) {
    mix_panned_mono (in[i], &(out[0]), &(out[1]), panI[i], faderI[i]);
    mix_aux_mono (in[i], &(out[2]), &(out[3]), aux1I[i], aux2I[i]);
    simple_busmix (effectBus, in[i],effectI[i]);
  }

  i=2;
  mix_panned_mono (selectGrainInput(sourceMixer3), &(out[0]), &(out[1]), panI[i], faderI[i]);
  mix_aux_mono (selectGrainInput(sourceMixer3), &(out[2]), &(out[3]), aux1I[i], aux2I[i]);
  simple_busmix (effectBus, selectGrainInput(sourceMixer3),effectI[i]);
  i=3;
  mix_panned_mono (selectGrainInput(sourceMixer4), &(out[0]), &(out[1]), panI[i], faderI[i]);
  mix_aux_mono (selectGrainInput(sourceMixer4), &(out[2]), &(out[3]), aux1I[i], aux2I[i]);
  simple_busmix (effectBus, selectGrainInput(sourceMixer4),effectI[i]);

  effectBusFeedback = 0;
  fract32 AMOut;
  fract32 grainOut;
  for (i=0;i<NGRAINS;i++) {
    grainOut=phaseG[i] * grain_next(&(grains[i]),
				    selectGrainInput(sourceG[i]),
				    mult_fr1x32x32(selectGrainInput(FM_sourceG[i]),
						   FM_faderG[i]));
    grainOut = lpf_next_dynamic(&(grain_lpf[i]),
				  grainOut,
				  grain_lpf_freq[i]);

    // FIXME AM signal comes out very quiet should be preceded by a 50Hz HPF
    // Then multiplied up by 20dB or so...
    // This is done now - but not quite sure about the results...
    AMOut = mult_fr1x32x32( mult_fr1x32x32 (selectGrainInput(AM_sourceG[i]),
					    grainOut),
			    AM_faderG[i]);
    AMOut = mult_fr1x32x32(hpf_next_dynamic(&(AM_hpf[i]), AMOut, hzToDimensionless(50)),
			   AM_faderG[i]);
    grainOut = mult_fr1x32x32(grainOut, sub_fr1x32(FR32_MAX, AM_faderG[i]));
    grainOut = add_fr1x32(shl_fr1x32(AMOut, 3), grainOut);

    grainOutFeedback[i] = grainOut;

    patchMatrix[5+i] = grainOut;
    patchMatrix[5+i+NGRAINS] = read_pitchTrackOsc(&(grains[i]));
    patchMatrix[5+i+NGRAINS+NGRAINS] = read_grainEnv(&(grains[i]));
    mix_panned_mono (grainOut, &(out[0]), &(out[1]), panG[i], faderG[i]);
    mix_aux_mono (grainOut, &(out[2]), &(out[3]), aux1G[i], aux2G[i]);
    simple_busmix (effectBusFeedback,
		   grainOut,
		   effectG[i]);
  }
  patchMatrix[0] = effectBus;

  fract32 phase_next = phasor_next(&LFO);
  patchMatrix[5+NGRAINS+NGRAINS+NGRAINS] =
    add_fr1x32(mult_fr1x32x32(LFO_shape,
			      osc(phase_next)),
	       mult_fr1x32x32(sub_fr1x32(FR32_MAX,LFO_shape),
			      osc_triangle(phase_next)));

  noiseBurstEnv = mult_fr1x32x32(noiseBurstEnv, FR32_MAX - noiseBurstDecay);
  patchMatrix[6+NGRAINS+NGRAINS+NGRAINS] =
    abs_fr1x32(mult_fr1x32x32(noiseBurstEnv,
			      lcprng_next (&noiseBurstSource)));

  simple_slew(CV_gen1, CV_gen1Target, SLEW_100MS);
  simple_slew(CV_gen2, CV_gen2Target, SLEW_100MS);
  patchMatrix[7+NGRAINS+NGRAINS+NGRAINS] = CV_gen1;
  patchMatrix[8+NGRAINS+NGRAINS+NGRAINS] = CV_gen2;
  process_cv();
}


// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE;
  switch(idx) {
    // ADC mix params
  case eParam_fader_i1 :
    faderITarget[0] = v;
    break;
  case eParam_pan_i1 :
    panITarget[0] = v;
    break;
  case eParam_aux1_i1 :
    aux1ITarget[0] = v;
    break;
  case eParam_aux2_i1 :
    aux2ITarget[0] = v;
    break;
  case eParam_effect_i1 :
    effectITarget[0] = v;
    break;

  case eParam_fader_i2 :
    faderITarget[1] = v;
    break;
  case eParam_pan_i2 :
    panITarget[1] = v;
    break;
  case eParam_aux1_i2 :
    aux1ITarget[1] = v;
    break;
  case eParam_aux2_i2 :
    aux2ITarget[1] = v;
    break;
  case eParam_effect_i2 :
    effectITarget[1] = v;
    break;

  case eParam_source_i3 :
    sourceMixer3 = v >> 16;
    break;
  case eParam_fader_i3 :
    faderITarget[2] = v;
    break;
  case eParam_pan_i3 :
    panITarget[2] = v;
    break;
  case eParam_aux1_i3 :
    aux1ITarget[2] = v;
    break;
  case eParam_aux2_i3 :
    aux2ITarget[2] = v;
    break;
  case eParam_effect_i3 :
    effectITarget[2] = v;
    break;

  case eParam_source_i4 :
    sourceMixer4 = v >> 16;
    break;
  case eParam_fader_i4 :
    faderITarget[3] = v;
    break;
  case eParam_pan_i4 :
    panITarget[3] = v;
    break;
  case eParam_aux1_i4 :
    aux1ITarget[3] = v;
    break;
  case eParam_aux2_i4 :
    aux2ITarget[3] = v;
    break;
  case eParam_effect_i4 :
    effectITarget[3] = v;
    break;

  //grain mix params
  case eParam_source_g1 :
    sourceG[0] = v >> 16;
    break;
  case eParam_fader_g1 :
    faderGTarget[0] = v;
    break;
  case eParam_pan_g1 :
    panGTarget[0] = v;
    break;
  case eParam_aux1_g1 :
    aux1GTarget[0] = v;
    break;
  case eParam_aux2_g1 :
    aux2GTarget[0] = v;
    break;
  case eParam_effect_g1 :
    effectGTarget[0] = v;
    break;
  case eParam_phase_g1 :
    if (v == 0)
      phaseG[0] = -1;
    else
      phaseG[0] = 1;
    break;

    //grain mod params
  case eParam_FM_level_g1 :
    FM_faderG[0] = v;
    break;
  case eParam_FM_source_g1 :
    FM_sourceG[0] = v >> 16;
    break;
  case eParam_AM_level_g1 :
    AM_faderG[0] = v;
    break;
  case eParam_AM_source_g1 :
    AM_sourceG[0] = v >> 16;
    break;
  case eParam_lpf_g1 :
    grain_lpf_freq[0] = hzToDimensionless((v >> 16));
    break;
    
    //grain scrubber params
  case eParam_scrubEnable_g1 :
    if(v ==0)
      grain_disable_scrubTap (&grains[0]);
    else
      grain_enable_scrubTap(&grains[0]);
    break;
  case eParam_scrubPitch_g1 :
    grain_set_scrubPitch(&(grains[0]), v >> 8);
    break;
  case eParam_scrubLength_g1 :
    grain_set_scrubLength (&(grains[0]), v >> 8);
    break;
  case eParam_scrubPitchDetection_g1:
    if (v == 0)
      grain_disable_pitchDetection(&(grains[0]));
    else
      grain_enable_pitchDetection(&(grains[0]));
    break;

  //grain echo params
  case eParam_echoFadeLength_g1 :
    grain_set_echoFadeLength(&(grains[0]), v << 7);
    break;
  case eParam_echoTime_g1 :
    grain_set_echoTime(&(grains[0]),v >> 4);
    break;
  case eParam_echoSpeed_g1 :
    grain_set_echoSpeed(&(grains[0]),v >> 8);
    break;
  case eParam_echoEdgeBehaviour_g1 :
    grain_set_echoEdgeBehaviour(&(grains[0]),v >> 16);
    break;
  case eParam_echoMin_g1 :
    grain_set_echoMin(&(grains[0]),v >> 4);
    break;
  case eParam_echoMax_g1 :
    grain_set_echoMax(&(grains[0]),v >> 4);
    break;
  case eParam_writeEnable_g1 :
    grain_set_writeEnable(&(grains[0]),v);
    break;
  case eParam_envAttack_g1 :
    grain_set_envAttack(&(grains[0]),shl_fr1x32(v, 4));
    break;

  case eParam_trackingEnv_g1 :
    if (v == 0)
      grain_disable_trackingEnv(&(grains[0]));
    else
      grain_enable_trackingEnv(&(grains[0]));
    break;
  case eParam_trackingPitch_g1 :
    grain_set_pitchOffset(&(grains[0]), shl_fr1x32(v, 13));
    break;

  //grain mix params
  case eParam_source_g2 :
    sourceG[1] = v >> 16;
    break;
  case eParam_fader_g2 :
    faderGTarget[1] = v;
    break;
  case eParam_pan_g2 :
    panGTarget[1] = v;
    break;
  case eParam_aux1_g2 :
    aux1GTarget[1] = v;
    break;
  case eParam_aux2_g2 :
    aux2GTarget[1] = v;
    break;
  case eParam_effect_g2 :
    effectGTarget[1] = v;
    break;
  case eParam_phase_g2 :
    if (v == 0)
      phaseG[1] = -1;
    else
      phaseG[1] = 1;
    break;

    //grain mod params
  case eParam_FM_level_g2 :
    FM_faderG[1] = v;
    break;
  case eParam_FM_source_g2 :
    FM_sourceG[1] = v >> 16;
    break;
  case eParam_AM_level_g2 :
    AM_faderG[1] = v;
    break;
  case eParam_AM_source_g2 :
    AM_sourceG[1] = v >> 16;
    break;
  case eParam_lpf_g2 :
    grain_lpf_freq[1] = hzToDimensionless((v >> 16));
    break;
    
    //grain scrubber params
  case eParam_scrubEnable_g2 :
    if(v ==0)
      grain_disable_scrubTap (&grains[1]);
    else
      grain_enable_scrubTap(&grains[1]);
    break;
  case eParam_scrubPitch_g2 :
    grain_set_scrubPitch(&(grains[1]), v>>8);
    break;
  case eParam_scrubLength_g2 :
    grain_set_scrubLength (&(grains[1]), v>>8);
    break;
  case eParam_scrubPitchDetection_g2:
    if (v == 0)
      grain_disable_pitchDetection(&(grains[1]));
    else
      grain_enable_pitchDetection(&(grains[1]));
    break;
    
  //grain echo params
  case eParam_echoFadeLength_g2 :
    grain_set_echoFadeLength(&(grains[1]), v<<7);
    break;
  case eParam_echoTime_g2 :
    grain_set_echoTime(&(grains[1]),v>>4);
    break;
  case eParam_echoSpeed_g2 :
    grain_set_echoSpeed(&(grains[1]),v>>8);
    break;
  case eParam_echoEdgeBehaviour_g2 :
    grain_set_echoEdgeBehaviour(&(grains[1]),v>>16);
    break;
  case eParam_echoMin_g2 :
    grain_set_echoMin(&(grains[1]),v>>4);
    break;
  case eParam_echoMax_g2 :
    grain_set_echoMax(&(grains[1]),v>>4);
    break;
  case eParam_writeEnable_g2 :
    grain_set_writeEnable(&(grains[1]),v);
    break;
  case eParam_envAttack_g2 :
    grain_set_envAttack(&(grains[1]),shl_fr1x32(v, 4));
    break;

  case eParam_trackingEnv_g2 :
    if (v == 0)
      grain_disable_trackingEnv(&(grains[1]));
    else
      grain_enable_trackingEnv(&(grains[1]));
    break;
  case eParam_trackingPitch_g2 :
    grain_set_pitchOffset(&(grains[1]), shl_fr1x32(v, 13));
    break;

  case eParam_LFO_speed :
    LFO.freq = shl_fr1x32(v, -4);
    break;
  case eParam_LFO_shape :
    LFO_shape = v;
    break;

  case eParam_noiseBurst :
    noiseBurstEnv = FR32_MAX >> 1;
    break;
  case eParam_noiseBurstDecay :
    noiseBurstDecay = v;
    break;

  case eParam_cvPatch1 :
    cvPatch[0] = v >> 16;
    break;
  case eParam_cvPatch2 :
    cvPatch[1] = v >> 16;
    break;
  case eParam_cvPatch3 :
    cvPatch[2] = v >> 16;
    break;
  case eParam_cvPatch4 :
    cvPatch[3] = v >> 16;
    break;

  case eParam_CV_gen1 :
    CV_gen1Target = v ;
    break;
  case eParam_CV_gen2 :
    CV_gen2Target = v ;
    break;

  default:
    break;
  }
}
