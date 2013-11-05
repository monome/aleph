/* tape.c
   aleph-audio

   tape-like buffer manipulation module.

 */

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "simple_string.h"
#include "types.h"

#ifdef ARCH_BFIN // bfin
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>
#else // linux
#include "fract32_emu.h"
#include "audio.h"
#endif

// audio
#include "buffer.h"
#include "filter_1p.h"
#include "module.h"

//-----------------------
//------ static variables
#if ARCH_LINUX
static FILE* dbgFile;
u8 dbgFlag = 0;
u32 dbgCount = 0;
#endif

// buffer size: 
// 2** 19  ~= 11sec at 48k
#define ECHO_BUF_SIZE 0x80000
//#define ECHO_BUF_SIZE_1 0x7ffff

//---- param ranges

// rate
#define RATE_MIN 0x2000 // 1/8
#define RATE_MAX 0x80000 // 8

// time
#define TIME_MIN 0
// 11 secs in 16.16
#define TIME_MAX 0xb0000

///// inputs
enum params {
  eParamAmp,
  eParamDry,
  eParamTime,
  eParamRate,
  eParamFb,
  eParamAmpSmooth,
  eParamTimeSmooth,
  eParamRateSmooth,
  eParamNumParams
};

typedef struct _tapeData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
  fract32 echoBufData[ECHO_BUF_SIZE];
} tapeData;

//-------------------------
//----- extern vars (initialized here)
moduleData* gModuleData; 

//-----------------------
//------ static variables

// pointer to all external memory
static tapeData* pTapeData;

//-- audio buffer class
static audioBuffer echoBuf;
//-- read and write taps
static bufferTapN tapRd;
static bufferTapN tapWr;

//-- current param values
static fix16   amp;
static fix16   dry;
static fix16   time;   // delay time between heads, in seconds
static fix16   rate;   // tape speed ratio
static fract32   fb;

// final output value
static fract32   frameVal;      
// echo output
static fract32   echoVal;

//-- param smoothers 
static filter_1p_fix16* ampLp;  // 1plp smoother for amplitude
static filter_1p_fix16* timeLp;  // 1plp smoother for delay time
static filter_1p_fix16* rateLp;  // 1plp smoother for "tape speed"

#ifdef ARCH_BFIN // bfin
#else // linux : emulate bfin audio core
fract32 in[0], in[1], in[2], in[3];
#endif

//----------------------
//----- static functions

// set all parameter data
static void fill_param_desc(void) {
  strcpy(gModuleData->paramDesc[eParamAmp].label, "amp");
  strcpy(gModuleData->paramDesc[eParamAmp].unit, "v");
  gModuleData->paramDesc[eParamAmp].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAmp].min = 0;
  gModuleData->paramDesc[eParamAmp].max = 0x10000;
  
  strcpy(gModuleData->paramDesc[eParamDry].label, "dry");
  strcpy(gModuleData->paramDesc[eParamDry].unit, "v");
  gModuleData->paramDesc[eParamDry].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamDry].min = 0;
  gModuleData->paramDesc[eParamDry].max = 0x10000;

  strcpy(gModuleData->paramDesc[eParamTime].label, "time");
  strcpy(gModuleData->paramDesc[eParamTime].unit, "v");
  gModuleData->paramDesc[eParamTime].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamTime].min = TIME_MIN;
  gModuleData->paramDesc[eParamTime].max = TIME_MAX;

  strcpy(gModuleData->paramDesc[eParamRate].label, "rate");
  strcpy(gModuleData->paramDesc[eParamRate].unit, "r");
  gModuleData->paramDesc[eParamRate].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamRate].min = RATE_MIN;
  gModuleData->paramDesc[eParamRate].max = RATE_MAX;

  strcpy(gModuleData->paramDesc[eParamFb].label, "feedback");
  strcpy(gModuleData->paramDesc[eParamFb].unit, "v");
  gModuleData->paramDesc[eParamFb].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamFb].min = 0;
  gModuleData->paramDesc[eParamFb].max = 0x10000;

  strcpy(gModuleData->paramDesc[eParamAmpSmooth].label, "amp smoothing");
  strcpy(gModuleData->paramDesc[eParamAmpSmooth].unit, "hz");
  gModuleData->paramDesc[eParamAmpSmooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamAmpSmooth].min = 0x8000;
  gModuleData->paramDesc[eParamAmpSmooth].max = 0x1000000;

  strcpy(gModuleData->paramDesc[eParamTimeSmooth].label, "time smoothing");
  strcpy(gModuleData->paramDesc[eParamTimeSmooth].unit, "hz");
  gModuleData->paramDesc[eParamTimeSmooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamTimeSmooth].min = 0x8000;
  gModuleData->paramDesc[eParamTimeSmooth].max = 0x1000000;

  strcpy(gModuleData->paramDesc[eParamRateSmooth].label, "rate smoothing");
  strcpy(gModuleData->paramDesc[eParamRateSmooth].unit, "hz");
  gModuleData->paramDesc[eParamRateSmooth].type = PARAM_TYPE_FIX;
  gModuleData->paramDesc[eParamRateSmooth].min = 0x8000;
  gModuleData->paramDesc[eParamRateSmooth].max = 0x1000000;

}


// set time
static inline void set_time(fix16 t) {  
  if( t < TIME_MIN ) { t = TIME_MIN; }
  if( t > TIME_MAX ) { t = TIME_MAX; }
  filter_1p_fix16_in(timeLp, t);
  //  time = t;
  //  buffer_tapN_sync(&tapRd, &tapWr, time);
}

// frame calculation
static void calc_frame(void) {
  // ----- smoothers:
  // amp
  amp = filter_1p_fix16_next(ampLp);
  // time
  if(timeLp->sync) {
    ;;
  } else {
    time = filter_1p_fix16_next(timeLp);
    buffer_tapN_sync(&tapRd, &tapWr, time);

#if ARCH_LINUX
      if(dbgFlag) {  
	fprintf(dbgFile, "%d \t %f \r\n", 
		dbgCount, 
		fix16_to_float(time)
		);
	dbgCount++;
      }

#endif


  }

  // rate
  //// NOTE: setting a different rate is pretty much pointless in this simple application.
  /// leaving it in just to test fractional interpolation methods.
  if(rateLp->sync) {
    ;;
  } else {
    //    rate = filter_1p_fix16_next(rateLp);
    //    buffer_tapN_set_rate(&tapRd, rate);
    //    buffer_tapN_set_rate(&tapWr, rate);
  }
  
  // get interpolated echo value
  echoVal = buffer_tapN_read(&tapRd);

  /* // store interpolated input+fb value */
  //  buffer_tap_write(&tapWr, add_fr1x32(in[0], mult_fr1x32x32(echoVal, fb ) ) );
  buffer_tapN_write(&tapWr, add_fr1x32(in[0], mult_fr1x32x32(echoVal, fb ) ) );

  //FIXME: clip / saturate input buf here
  //// potentially, scale input by inverse feedback

 /// test: no fb
  //buffer_tap_write(&tapWr, in[0]);

  /* // output */
  frameVal = add_fr1x32(
  			mult_fr1x32x32( echoVal, FIX16_FRACT_TRUNC(amp) ),
  			mult_fr1x32x32( in[0],  FIX16_FRACT_TRUNC(dry) )
  			 );
  //// test: no dry
  //  frameVal = echoVal
  /// FIXME: clip here

  buffer_tapN_next(&tapRd);
  buffer_tapN_next(&tapWr);
}

//----------------------
//----- external functions

void module_init(void) {

  // init module/param descriptor
#ifdef ARCH_BFIN 
  pTapeData = (tapeData*)SDRAM_ADDRESS;
#else
  pTapeData = (tapeData*)malloc(sizeof(tapeData));
  // debug file
  dbgFile = fopen( "tape_dbg.txt", "w");
#endif
  
  gModuleData = &(pTapeData->super);
  gModuleData->paramDesc = pTapeData->mParamDesc;
  gModuleData->paramData = pTapeData->mParamData;
  gModuleData->numParams = eParamNumParams;

  fill_param_desc();
  
  // init params
  amp = FIX16_ONE >> 1;
  time = FIX16_ONE << 1;
  rate = FIX16_ONE;
  fb = 0;

  // init smoothers
  ampLp = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init( ampLp, SAMPLERATE, fix16_from_int(32), amp );
  
  timeLp = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init( timeLp, SAMPLERATE, fix16_from_int(32), time );
  
  rateLp = (filter_1p_fix16*)malloc(sizeof(filter_1p_fix16));
  filter_1p_fix16_init( rateLp, SAMPLERATE, fix16_from_int(32), time );

  // init buffer and taps
  buffer_init(&echoBuf, pTapeData->echoBufData, ECHO_BUF_SIZE);
  buffer_tapN_init(&tapRd, &echoBuf);
  buffer_tapN_init(&tapWr, &echoBuf);

  // set rate and offset
  //  buffer_tapN_set_inc(&tapRd, rate);
  //  buffer_tapN_set_inc(&tapWr, rate);
  buffer_tapN_sync(&tapRd, &tapWr, time);
}

// de-init
void module_deinit(void) {
  free(ampLp);
  free(timeLp);
  free(rateLp);
}

// set parameter by value (fix16)
void module_set_param(u32 idx, pval v) {
  switch(idx) {
  case eParamAmp:
    filter_1p_fix16_in(ampLp, v.fix);
    break;
  case eParamDry:
    dry = v.fix;
    break;
  case eParamTime:
    set_time(v.fix);
    break;
  case eParamRate:
    filter_1p_fix16_in(rateLp, v.fix);
    break;
  case eParamFb:
    fb =  FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParamAmpSmooth:
    filter_1p_fix16_set_hz(ampLp, v.fix);
    break;
  case eParamTimeSmooth:
    filter_1p_fix16_set_hz(timeLp, v.fix);
    break;
  case eParamRateSmooth:
    filter_1p_fix16_set_hz(rateLp, v.fix);
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



  //// test: mix all 
  /* in[0] = add_fr1x32(in[0], */
  /* 		   add_fr1x32(in1, */
  /* 			      add_fr1x32(in2, in3) ) ); */
  
  calc_frame();

  out[0] = frameVal;
  out[1] = frameVal;
  out[2] = frameVal;
  out[3] = frameVal; 

  /* /// test: wire, after processing */
  /* out0 = in0; */
  /* out1 = in1; */
  /* out2 = in2; */
  /* out3 = in3; */


}

static u8 ledstate = 0;
u8 module_update_leds(void) {
  return ledstate;
}

#else //  non-bfin
void module_process_frame(const f32* in, f32* out) {

  static fract32* const pIn[4] = { &in0, &in1, &in2, &in3 };
  u32 frame;
  u8 chan;
  // i/o buffers are (sometimes) interleaved in portaudio,
  // so need to introduce 1-sample delay for x-channel processing
  //  static int _in0, _in1, _in2, _in3;
  for(frame=0; frame<BLOCKSIZE; frame++) {
    calc_frame();
    for(chan=0; chan<NUMCHANNELS; chan++) { // stereo interleaved
      *out++ = fr32_to_float(frameVal);
      *(pIn[chan]) = float_to_fr32(*in++);
    }
  }
}
#endif


