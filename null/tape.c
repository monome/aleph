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
#include "filters.h"
#include "module.h"

//-----------------------
//------ static variables
#if ARCH_LINUX
static FILE* dbgFile;
u8 dbgFlag = 0;
u32 dbgCount = 0;
#endif

// buffer size: 
// 2** 19  ~= 11sec at 44.1k
#define ECHO_BUF_SIZE 0x80000
#define ECHO_BUF_SIZE_1 0x7ffff

//---- param ranges

// rate
#define RATE_MIN 0x2000 // 1/8
#define RATE_MAX 0x80000 // 8

// time
/// FIXME: use buffer class to calculate this based on size/sr
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
static bufferTap tapRd;
static bufferTap tapWr;

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
fract32 in0, in1, in2, in3;
#endif

//----------------------
//----- static functions

// set time
static inline void set_time(fix16 t) {  
  if( t < TIME_MIN ) { t = TIME_MIN; }
  if( t > TIME_MAX ) { t = TIME_MAX; }
  filter_1p_fix16_in(timeLp, t);
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
    buffer_tap_sync(&tapRd, &tapWr, time);

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
    rate = filter_1p_fix16_next(rateLp);
    buffer_tap_set_rate(&tapRd, rate);
    buffer_tap_set_rate(&tapWr, rate);
  }
  
  // get interpolated echo value

  echoVal = buffer_tap_read(&tapRd);

  /* // store interpolated input+fb value */
  //  buffer_tap_write(&tapWr, add_fr1x32(in0, mult_fr1x32x32(echoVal, fb ) ) );
  buffer_tap_write(&tapWr, add_fr1x32(in0 >> 1, mult_fr1x32x32(echoVal, fb ) ) );

  //FIXME: clip / saturate input buf here
  //// potentially, scale input by inverse feedback

 /// test: no fb
  //buffer_tap_write(&tapWr, in0);

  /* // output */
  frameVal = add_fr1x32(
  			mult_fr1x32x32( echoVal, FIX16_FRACT_TRUNC(amp) ),
  			mult_fr1x32x32( in0,  FIX16_FRACT_TRUNC(dry) )
  			 );
  //// test: no dry
  //  frameVal = echoVal;
  /// FIXME: clip here

  buffer_tap_next(&tapRd);
  buffer_tap_next(&tapWr);
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

  gModuleData->numParams = eParamNumParams;
  gModuleData->paramDesc = (ParamDesc*)malloc(eParamNumParams * sizeof(ParamDesc));

  
  strcpy(gModuleData->paramDesc[eParamAmp].label, "amp");
  strcpy(gModuleData->paramDesc[eParamDry].label, "dry");
  strcpy(gModuleData->paramDesc[eParamTime].label, "time");
  strcpy(gModuleData->paramDesc[eParamRate].label, "rate");
  strcpy(gModuleData->paramDesc[eParamFb].label, "feedback");
  strcpy(gModuleData->paramDesc[eParamAmpSmooth].label, "amp smoothing");
  strcpy(gModuleData->paramDesc[eParamTimeSmooth].label, "time smoothing");
  
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
  buffer_init(&echoBuf, pTapeData->echoBufData, ECHO_BUF_SIZE, SAMPLERATE);
  buffer_tap_init(&tapRd, &echoBuf);
  buffer_tap_init(&tapWr, &echoBuf);

  // set rate and offset
  buffer_tap_set_rate(&tapRd, rate);
  buffer_tap_set_rate(&tapWr, rate);
  buffer_tap_sync(&tapRd, &tapWr, time);
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
  calc_frame();
  out0 = frameVal;
  out1 = frameVal;
  out2 = frameVal;
  out3 = frameVal; 
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


