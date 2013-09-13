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
#warning "linux"
#include "audio.h"
#endif

#include "module.h"
#include "module_custom.h"
#include "types.h"

#define SVF_HZ_MIN 0x200000      // 32
#define SVF_HZ_MAX 0x40000000    // 16384


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
  frameVal = mult_fr1x32x32(noise_next(), noiseAmp);
  frameVal = add_fr1x32(frameVal, mult_fr1x32x32(in[0], inAmp0));
  frameVal = add_fr1x32(frameVal, mult_fr1x32x32(in[1], inAmp1));
  frameVal = add_fr1x32(frameVal, mult_fr1x32x32(in[2], inAmp2));
  frameVal = add_fr1x32(frameVal, mult_fr1x32x32(in[3], inAmp3));
  frameVal = filter_svf_next(svf, frameVal);
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
  
  // initial param state
  filter_svf_set_hz(svf, fix16_from_int(220));
  filter_svf_set_rq(svf, 0x4000);
  filter_svf_set_low(svf, 0x4000);

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

static void fill_param_desc(void) {

  /* strcpy(gModuleData->paramDesc[eParamSvfHz].label, "cutoff"); */
  /* strcpy(gModuleData->paramDesc[eParamSvfHz].unit, "hz"); */
  /* gModuleData->paramDesc[eParamSvfHz].type = PARAM_TYPE_FIX; */
  /* gModuleData->paramDesc[eParamSvfHz].min = SVF_HZ_MIN; */
  /* gModuleData->paramDesc[eParamSvfHz].max = SVF_HZ_MAX; */
  /// FIXME: svf corner coeff is dumb right now
  strcpy(gModuleData->paramDesc[eParamSvfHz].label, "cutoff");
  strcpy(gModuleData->paramDesc[eParamSvfHz].unit, "");
  gModuleData->paramDesc[eParamSvfHz].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfHz].min = 0;
  gModuleData->paramDesc[eParamSvfHz].max = 0x8000;

  strcpy(gModuleData->paramDesc[eParamSvfRq].label, "rq");
  strcpy(gModuleData->paramDesc[eParamSvfRq].unit, "");
  gModuleData->paramDesc[eParamSvfRq].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfRq].min = 0;
  gModuleData->paramDesc[eParamSvfRq].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfLow].label, "low");
  strcpy(  gModuleData->paramDesc[eParamSvfLow].unit, "");
  gModuleData->paramDesc[eParamSvfLow].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfLow].min = 0;
  gModuleData->paramDesc[eParamSvfLow].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfHigh].label, "high");
  strcpy(  gModuleData->paramDesc[eParamSvfHigh].unit, "");
  gModuleData->paramDesc[eParamSvfHigh].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfHigh].min = 0;
  gModuleData->paramDesc[eParamSvfHigh].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfBand].label, "band");
  strcpy(  gModuleData->paramDesc[eParamSvfBand].unit, "");
  gModuleData->paramDesc[eParamSvfBand].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfBand].min = 0;
  gModuleData->paramDesc[eParamSvfBand].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfNotch].label, "notch");
  strcpy(  gModuleData->paramDesc[eParamSvfNotch].unit, "");
  gModuleData->paramDesc[eParamSvfNotch].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfNotch].min = 0;
  gModuleData->paramDesc[eParamSvfNotch].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamSvfPeak].label, "peak");
  strcpy(  gModuleData->paramDesc[eParamSvfPeak].unit, "");
  gModuleData->paramDesc[eParamSvfPeak].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamSvfPeak].min = 0;
  gModuleData->paramDesc[eParamSvfPeak].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamNoiseAmp].label, "noise");
  strcpy(  gModuleData->paramDesc[eParamNoiseAmp].unit, "");
  gModuleData->paramDesc[eParamNoiseAmp].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamNoiseAmp].min = 0;
  gModuleData->paramDesc[eParamNoiseAmp].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamInAmp0].label, "in0");
  strcpy(  gModuleData->paramDesc[eParamInAmp0].unit, "");
  gModuleData->paramDesc[eParamInAmp0].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamInAmp0].min = 0;
  gModuleData->paramDesc[eParamInAmp0].max = fix16_one;

  strcpy( gModuleData->paramDesc[eParamInAmp1].label, "in1");
  strcpy(  gModuleData->paramDesc[eParamInAmp1].unit, "");
  gModuleData->paramDesc[eParamInAmp1].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamInAmp1].min = 0;
  gModuleData->paramDesc[eParamInAmp1].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamInAmp2].label, "in2");
  strcpy(  gModuleData->paramDesc[eParamInAmp2].unit, "");
  gModuleData->paramDesc[eParamInAmp2].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamInAmp2].min = 0;
  gModuleData->paramDesc[eParamInAmp2].max = fix16_one;

  strcpy(gModuleData->paramDesc[eParamInAmp3].label, "in3");
  strcpy(  gModuleData->paramDesc[eParamInAmp3].unit, "");
  gModuleData->paramDesc[eParamInAmp3].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamInAmp3].min = 0;
  gModuleData->paramDesc[eParamInAmp3].max = fix16_one;
}


