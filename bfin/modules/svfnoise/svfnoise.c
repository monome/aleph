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
#include "audio.h"
#endif

#include "module.h"
#include "types.h"

#define SVF_HZ_MIN 0x200000      // 32
#define SVF_HZ_MAX 0x40000000    // 16384

enum params {
  eParamSvfHz,
  eParamSvfResonance,
  eParamSvfLow,
  eParamSvfHigh,
  eParamSvfBand,
  eParamSvfNotch,
  eParamSvfPeak,
  eParamNoiseAmp,
  eParamInAmp0,
  eParamInAmp1,
  eParamInAmp2,
  eParamInAmp3,
  eParamNumParams
};


// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
typedef struct _svfNoiseData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
} svfNoiseData;


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


// pointer to local module data, initialize at top of SDRAM
static svfNoiseData * data;

//-- static allocation (SRAM) for variables that are small and/or frequently accessed:
// two random number generators for low/high words of very white noise:
static lcprng* rngH;
static lcprng* rngL;
// filter
static filter_svf* svf;
// parameters
static fract32 inAmp0;
static fract32 inAmp1;
static fract32 inAmp2;
static fract32 inAmp3;
static fract32 noiseAmp;
// output
static fract32 frameVal;

//-----------------------------
//----- static functions

// get the next value of white noise
static fract32 noise_next(void) {
  fract32 x = 0;
  x |= lcprng_next(rngH);
  x |= ((lcprng_next(rngL) >> 16) & 0x0000ffff);
  return x;
}

// frame calculation
static void calc_frame(void) {
  frameVal = 0;
}

// set all parameter descriptors
static void fill_param_desc(void);


//----------------------
//----- external functions

void module_init(void) {
  // init module/param descriptor
#ifdef ARCH_BFIN 
  // intialize local data at start of SDRAM
  data = (svfNoiseData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
#else
  data = (svfNoiseData*)malloc(sizeof(svfNoiseData));
  /// debugging output file
  dbgFile = fopen( "iotest_dbg.txt", "w");
#endif
  gModuleData = &(data->super);
  gModuleData->paramDesc = data->mParamDesc;
  gModuleData->paramData = data->mParamData;
  gModuleData->numParams = eParamNumParams;

  fill_param_desc();

  inAmp0 = 0;
  inAmp1 = 0;
  inAmp2 = 0;
  inAmp3 = 0;
  noiseAmp = fix16_one >> 2;

  svf = (filter_svf*)malloc(sizeof(filter_svf));
  filter_svf_init(svf);

  rngH = (lcprng*)malloc(sizeof(lcprng));
  lcprng_reset(rngH);

  rngL = (lcprng*)malloc(sizeof(lcprng));
  lcprng_reset(rngL);
}

void module_deinit(void) {
  free(svf);
  free(rngH);
  free(rngL);

#if ARCH_LINUX 
  fclose(dbgFile);
#endif
}


// set parameter by value (fix16)
void module_set_param(u32 idx, pval v) {
  switch(idx) {
  /* case eParamHz1: */
  /*   set_hz1(v.fix); */
  /*   break; */
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
  out0 = (frameVal);
  out1 = (frameVal);
  out2 = (frameVal);
  out3 = (frameVal);
  /* out0 = add_fr1x32(frameVal, mult_fr1x32x32(in0, ioAmp0)); */
  /* out1 = add_fr1x32(frameVal, mult_fr1x32x32(in1, ioAmp1)); */
  /* out2 = add_fr1x32(frameVal, mult_fr1x32x32(in2, ioAmp2)); */
  /* out3 = add_fr1x32(frameVal, mult_fr1x32x32(in3, ioAmp3)); */
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

static void fill_param_desc(void) {
  /* strcpy(gModuleData->paramDesc[eParamHz1].label, "osc 1 freq"); */
  /* strcpy(gModuleData->paramDesc[eParamHz1].unit, "hz"); */
  /* gModuleData->paramDesc[eParamHz1].type = PARAM_TYPE_FIX; */
  /* gModuleData->paramDesc[eParamHz1].min = OSC_HZ_MIN; */
  /* gModuleData->paramDesc[eParamHz1].max = OSC_HZ_MAX; */


}


