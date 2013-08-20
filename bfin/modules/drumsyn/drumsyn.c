/* svfnoise.c
   aleph-bfin
   
   noise generator + input + state variable filter

*/

// std
#include <stdlib.h>
#include <string.h>
// aleph-common
#include "fix.h"
// aleph-audio
#include "conversion.h"
#include "env.h"
#include "filter_svf.h"
#include "noise.h"
// bfin
#ifdef ARCH_BFIN
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>
#else
// linux
#include <stdio.h>
#include "fract32_emu.h"
#warning "linux"
#include "audio.h"
#endif

#include "module.h"
#include "module_custom.h"
#include "types.h"

#define HZ_MIN 0x200000      // 32
#define HZ_MAX 0x40000000    // 16384


// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
typedef struct _drumsData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
} drumsData;

typedef struct _drum_voice {
  filter_svf* svf;
  env_asr* envAmp;
  env_asr* envHz;
  env_asr* envRes;
  lcprng* rngH;
  lcprng* rngL;
} drum_voice_t;

//-------------------------
//----- extern vars (initialized here)
moduleData * gModuleData; // module data

//-----------------------
//------ static variables
#if ARCH_LINUX
static FILE* dbgFile;
u8 dbgFlag = 0;
u32 dbgCount = 0;
#endif

/// TEST: one voice
drumsyn_voice* voice[1];


// pointer to local module data, initialize at top of SDRAM
static drumsynData * data;
//-- static allocation (SRAM) for variables that are small and/or frequently accessed:
drumsyn_voice_t voice;

//-----------------------------
//----- static functions

// initialize voice
void drumsyn_voice_init(void* mem) {
  drum_voice_t* voice = (drumsyn_voice_t*)mem;

  // envelopes
  voice->envAmp = (env_asr*)malloc(sizeof(env_asr));
  env_asr_init(voice->envAmp);
  voice->envHz = (env_asr*)malloc(sizeof(env_asr));
  env_asr_init(voice->envHz);
  voice->envRes = (env_asr*)malloc(sizeof(env_asr));
  env_asr_init(voice->envRes);
  // SVF
  voice->svf = (filter_svf*)malloc(sizeof(filter_svf));
  filter_svf_init(voice->svf);
  // RNG
  voice->rngH = (lcprng*)malloc(sizeof(lcprng));
  lcprng_reset(voice->rngH);
  voice->rngL = (lcprng*)malloc(sizeof(lcprng));
  lcprng_reset(voice->rngL);
}

void drumsyn_voice_deinit(drumsyn_voice* voice) {
  free(voice->envAmp);
  free(voice->envHz);
  free(voice->envRes);
  free(voice->rngL);
  free(voice->rngH);
  free(voice->svf);
}

// next value of voice
fract32 drumsyn_voice_next(drumsyn_voice_t* voice) {
  return filter_svf_next(voice->svf, 
			 lcprng_next(voice->rngL) | ( lcprng_next(voice->rngH) << 16) );
}


// frame calculation
static void calc_frame(void) {
  frameVal = drumsym_voice_next(voice[0]);
}

//----------------------
//----- external functions

void module_init(void) {
  // init module/param descriptor
#ifdef ARCH_BFIN 
  // intialize local data at start of SDRAM
  data = (drumsData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
#else
  data = (drumsData*)malloc(sizeof(drumsData));
  /// debugging output file
  dbgFile = fopen( "drums_dbg.txt", "w");
#endif
  gModuleData = &(data->super);
  gModuleData->paramDesc = data->mParamDesc;
  gModuleData->paramData = data->mParamData;
  gModuleData->numParams = eParamNumParams;

}

void module_deinit(void) {
  drumsyn_voice_deinit(voice[0]);
#if ARCH_LINUX 
  fclose(dbgFile);
#endif
}


// set parameter by value (fix16)
void module_set_param(u32 idx, pval v) {
  switch(idx) {
  case eParamGate:
     env_asr_set_gate(ampEnv, v.s > 0);
    break;
  case eParamSvfHz :
    filter_svf_set_hz(svf, v.fix);
    break;
  case eParamSvfRq :
    filter_svf_set_rq(svf, FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfLow :
    filter_svf_set_low(svf, FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfHigh :
    filter_svf_set_high(svf, FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfBand :
    filter_svf_set_band(svf, FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfNotch :
    filter_svf_set_notch(svf, FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamSvfPeak :
    filter_svf_set_peak(svf, FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParamNoiseAmp :
    noiseAmp = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamInAmp0 :
    inAmp0 = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamInAmp1 :
    inAmp0 = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamInAmp2 :
    inAmp0 = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamInAmp3 :
    inAmp0 = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamAtkDur:
    env_asr_set_atk_dur(ampEnv, sec_to_frames_trunc(v.fix));
    break;
  case eParamRelDur:
    env_asr_set_rel_dur(ampEnv, sec_to_frames_trunc(v.fix));
    break;
  case eParamAtkCurve:
    env_asr_set_atk_shape(ampEnv, FIX16_FRACT_TRUNC(BIT_ABS(v.fix)));
    break;
  case eParamRelCurve:
    env_asr_set_atk_shape(ampEnv, FIX16_FRACT_TRUNC(BIT_ABS(v.fix)));
    break;

  default:
    break;
  }
}

// get number of parameters
extern u32 module_get_num_params(void) {
  return eParamNumParams;
}

// frame callback
#ifdef ARCH_BFIN 
void module_process_frame(void) {
  calc_frame();
  out[0] = (frameVal);
  out[1] = (frameVal);
  out[2] = (frameVal);
  out[3] = (frameVal);
}

#else //  non-bfin
void module_process_frame(const f32* in, f32* out) {
  u32 frame;
  u8 chan;
  for(frame=0; frame<BLOCKSIZE; frame++) {
    calc_frame(); 
    for(chan=0; chan<NUMCHANNELS; chan++) { // stereo interleaved
      // FIXME: could use fract for output directly (portaudio setting?)
      *out = fr32_to_float(frameVal);
      if(dbgFlag) {  
	/* fprintf(dbgFile, "%d \t %f \t %f \t %f \r\n",  */
	/* 	dbgCount,  */
	/* 	*out */
	/* 	//		fr32_to_float(osc2), */
	/* 	//		fr32_to_float((fract32)modIdxOffset << 3) */
	/* 	//		fr32_to_float((fract32)modIdxOffset << 16) */
	/* 	//		fr32_to_float((fract32)modIdxOffset) */
	/* 	); */
	dbgCount++;
      }
      out++;
    }
  }
}
#endif
