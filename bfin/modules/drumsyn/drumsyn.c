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
#warning "LINUX"
#include <stdio.h>
#include "fract32_emu.h"
#warning "linux"
#include "audio.h"
#endif

#include "module.h"
#include "module_custom.h"
#include "params.h"
#include "types.h"

#include "drumsyn.h"



#define HZ_MIN 0x200000      // 32
#define HZ_MAX 0x40000000    // 16384

// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
//#if 0
typedef struct _drumsData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
} drumsynData;
//#endif

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
drumsynVoice* voices[DRUMSYN_NVOICES];

// pointer to local module data, initialize at top of SDRAM
static drumsynData * data;
//-- static allocation (SRAM) for variables that are small and/or frequently accessed:
static fract32 frameVal;

//-----------------------------
//----- static functions

// initialize voice
void drumsyn_voice_init(void* mem) {
  drumsynVoice* voice = (drumsynVoice*)mem;

#ifdef DRUMSYN_NOENV
  // integrators
  filter_1p_fr32_init(&(voice->lpAmp), 0);
  filter_1p_fr32_init(&(voice->lpFreq), 0x100);
  filter_1p_fr32_init(&(voice->lpRq), 0x200);
#else

  // envelopes

    ///// /AHH rrg
  voice->envAmp = (env_int*)malloc(sizeof(env_int));
  env_int_init(voice->envAmp);

  voice->envFreq = (env_int*)malloc(sizeof(env_int));
  env_int_init(voice->envFreq);

  voice->envRq = (env_int*)malloc(sizeof(env_int));
  env_int_init(voice->envRq);

  env_int_set_scale(voice->envAmp, FR32_MAX >> 1);

#endif

  // SVF
  voice->svf = (filter_svf*)malloc(sizeof(filter_svf));
  filter_svf_init(voice->svf);

  filter_svf_set_low(voice->svf, FR32_MAX >> 1);
  filter_svf_set_coeff(voice->svf, FR32_MAX >> 2);
  filter_svf_set_rq(voice->svf, FR32_MAX >> 3);

  // RNG
  voice->rngH = (lcprng*)malloc(sizeof(lcprng));
  lcprng_reset(voice->rngH);
  voice->rngL = (lcprng*)malloc(sizeof(lcprng));
  lcprng_reset(voice->rngL);
}

void drumsyn_voice_deinit(drumsynVoice* voice) {
#ifdef DRUMSYN_NOENV
#else 
 free(voice->envAmp);
  free(voice->envFreq);
  free(voice->envRq);
  free(voice->rngL);
  free(voice->rngH);
  free(voice->svf);
#endif
}
// next value of voice
static fract32 drumsyn_voice_next(drumsynVoice* voice);
fract32 drumsyn_voice_next(drumsynVoice* voice) {
#ifdef DRUMSYN_NOENV
  fract32 amp, freq, rq;

    amp = filter_1p_fr32_next(&(voice->lpAmp));
    freq = filter_1p_fr32_next(&(voice->lpFreq));
    rq = filter_1p_fr32_next(&(voice->lpRq));

    filter_svf_set_coeff( voice->svf, freq );
    filter_svf_set_rq( voice->svf, rq );

#else 
  // FIXME : janky, need more voices
  fract32 ampenv, ampenvold, amp;
  fract32 freqenv, freqenvold;
  fract32 rqenv, rqenvold;
  freqenv = env_int_next ( voice->envFreq );
  if(freqenv != freqenvold) {
    /* filter_svf_set_coeff( voice->svf, add_fr1x32(voice->envAddFreq,  */
    /* 						 mult_fr1x32x32(freqenv, voice->envMulFreq) */
    /* 						 ) */
    /* 			  ); */
    filter_svf_set_coeff( voice->svf, add_fr1x32(voice->envAddFreq, freqenv) );
    freqenvold = freqenv;
  }
  rqenv = env_int_next ( voice->envRq );
  if(rqenv != rqenvold) {
    /* filter_svf_set_rq( voice->svf, add_fr1x32(voice->envAddRq,  */
    /* 					      mult_fr1x32x32(rqenv, voice->envMulRq) */
    /* 					      ) */
    /* 		       ); */
    filter_svf_set_rq( voice->svf, add_fr1x32(voice->envAddRq, rqenv) );
    rqenvold = rqenv;
  }
#endif


  /// FIXME : testing only amp env  
#if 0
  return mult_fr1x32x32( amp,
			 filter_svf_next(voice->svf, 
					 lcprng_next(voice->rngL) |
					 ( lcprng_next(voice->rngH) << 14) 
					 )
			 );
#else  // TEST
  return mult_fr1x32x32(amp, lcprng_next(voice->rngL) | ( lcprng_next(voice->rngH) << 14 ) );
#endif
}



// frame calculation
static void calc_frame(void) {
  frameVal = drumsyn_voice_next(voices[0]);
}

//----------------------
//----- external functions

void module_init(void) {
  u8 i;
  // init module/param descriptor
#ifdef ARCH_BFIN 
  // intialize local data at start of SDRAM
  data = (drumsynData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
#else
  data = (drumsynData*)malloc(sizeof(drumsData));
  /// debugging output file
  dbgFile = fopen( "drums_dbg.txt", "w");
#endif
  gModuleData = &(data->super);
  strcpy(gModuleData->name, "aleph-drumsyn");
  gModuleData->paramDesc = data->mParamDesc;
  gModuleData->paramData = data->mParamData;
  gModuleData->numParams = eParamNumParams;

  for(i=0; i<DRUMSYN_NVOICES; i++) {
    voices[i] = (drumsynVoice*)malloc(sizeof(drumsynVoice));
    drumsyn_voice_init(voices[i]);
  }

#if 0
  //////////
  /// TESTING
  filter_1p_fr32_init(&ampLp, 0);
  filter_1p_fr32_set_slew(&ampLp, 0x1000);
  ////////
  ///////
#endif 

  fill_param_desc();


  //// TEST



}

void module_deinit(void) {
  drumsyn_voice_deinit(voices[0]);
#if ARCH_LINUX 
  fclose(dbgFile);
#endif
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
