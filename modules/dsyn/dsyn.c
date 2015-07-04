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
//#include "noise.h"
#include "noise_dsyn.h"

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

#include "dsyn.h"

// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
typedef struct _drumsData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
} drumsynData;

//-------------------------
//----- extern vars (initialized here)
ModuleData * gModuleData; // module data

//-----------------------
//------ static variables
#if ARCH_LINUX
static FILE* dbgFile;
u8 dbgFlag = 0;
u32 dbgCount = 0;
#endif

drumsynVoice* voices[DRUMSYN_NVOICES];

// pointer to local module data, initialize at top of SDRAM
static drumsynData * data;
//-- static allocation (SRAM) for variables that are small and/or frequently accessed:
static fract32 frameVal;

//-----------------------------
//----- static functions

static void drumsyn_voice_init(void* mem, int i);
static fract32 drumsyn_voice_next(drumsynVoice* voice, int i);
static fract32 noise_next(drumsynVoice* voice, int i);

// get next noise-generator value
fract32 noise_next(drumsynVoice* voice, int i) {
  return dsyn_noise_next(i);
}

// initialize voice
void drumsyn_voice_init(void* mem, int i) {
  drumsynVoice* voice = (drumsynVoice*)mem;
  // svf
  filter_svf_init(&(voice->svf));

  // noise
  dsyn_noise_reset(i);
		   
  // hipass
  filter_2p_hi_init(&(voice->hipass));

  // envelopes
  env_exp_init(&(voice->envAmp));
  env_exp_init(&(voice->envFreq));
  env_exp_init(&(voice->envRq));

  // SVF
  filter_svf_init(&(voice->svf));

  filter_svf_set_low(&(voice->svf), FR32_MAX >> 1);
  filter_svf_set_coeff(&(voice->svf), FR32_MAX >> 2);
  filter_svf_set_rq(&(voice->svf), FR32_MAX >> 3);

  env_exp_set_off( &(voice->envAmp) , 0 );
  env_exp_set_on(  &(voice->envAmp) , FR32_MAX >> 2 );
  
  env_exp_set_off(  &(voice->envFreq) , 0xffff );
  env_exp_set_on(  &(voice->envFreq) , 0x1fffffff );

  env_exp_set_off(  &(voice->envRq) , 0x1fffffff );
  env_exp_set_on(  &(voice->envRq) , 0x1fffffff );

  voice->freqEnv = 1;
  voice->rqEnv = 1;
  voice->svfPre = 1;

}

void drumsyn_voice_deinit(drumsynVoice* voice) {
  //... nothing to do
}

// next value of voice
fract32 drumsyn_voice_next(drumsynVoice* voice, int i) {
  filter_svf* f = &(voice->svf);
  fract32 amp, freq, rq;

  amp = env_exp_next(&(voice->envAmp));

  if(voice->freqEnv > 0) {
    freq = env_exp_next(&(voice->envFreq));
    filter_svf_set_coeff( f, freq );
  }

  if(voice->rqEnv > 0) {
    rq = env_exp_next(&(voice->envRq));
    filter_svf_set_rq( f, rq );
  }

  if(voice->svfPre) {
    return shr_fr1x32(mult_fr1x32x32( amp, filter_svf_next(f, noise_next(voice, i) )) , 1);
    //    return mult_fr1x32x32( amp, filter_svf_next(f, shr_fr1x32(noise_next(voice, i), 1) ) );
  } else {
    return shr_fr1x32(filter_svf_next(f, mult_fr1x32x32( amp, noise_next(voice, i) )) , 1);
    //    return filter_svf_next(f, mult_fr1x32x32( amp, shl_fr1x32(noise_next(voice, i), 1) ) );
  }
}



// frame calculation
static void calc_frame(void) {
  fract32 dum;
  frameVal = shr_fr1x32(drumsyn_voice_next(voices[0], 0), 1);

  dum = drumsyn_voice_next(voices[1], 1);
  frameVal = add_fr1x32(frameVal, shr_fr1x32(dum, 1) );

  dum = drumsyn_voice_next(voices[2], 2);
  frameVal = add_fr1x32(frameVal, shr_fr1x32(dum, 1) );

  dum = drumsyn_voice_next(voices[3], 3);
  frameVal = add_fr1x32(frameVal, shr_fr1x32(dum, 1) );
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
  strcpy(gModuleData->name, "dsyn");

  gModuleData->paramData = data->mParamData;
  gModuleData->numParams = eParamNumParams;

  for(i=0; i<DRUMSYN_NVOICES; i++) {
    voices[i] = (drumsynVoice*)malloc(sizeof(drumsynVoice));
    drumsyn_voice_init(voices[i], i);
  }

  // setup params with default values
  params_default();

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
  // sum input pairs to output pairs
  fract32 sum01 = add_fr1x32(in[0], in[1]);
  fract32 sum23 = add_fr1x32(in[2], in[3]);

  // dsyn output is mono :(  
  // calculate frameVal
  calc_frame();

  sum01 = add_fr1x32(sum01, frameVal);
  sum23 = add_fr1x32(sum23, frameVal);

  out[0] = sum01;
  out[1] = sum01;
  out[2] = sum23;
  out[3] = sum23;
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
