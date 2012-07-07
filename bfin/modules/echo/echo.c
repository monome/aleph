#include <fract_math.h>
#include "bfin_core.h"
#include "param.h"
#include "module.h"
#include "echo_data.h"

//------- global variable definitions
// parameter data
ParamData paramData[MODULE_NUM_PARAMS];
// parameter description
ParamDesc paramDesc[MODULE_NUM_PARAMS];

//-------- local variables
// write frame
static u64 wFrame;
// in/out pointers (FIXME: could eliminate by unrolling)
static fract32* pOut[4];
static fract32* pIn[4];

// (these don't have to be global but might as well stay on stack)

// helper arrays for looping...
// FIXME: can unroll if necessary
static const u8 mixParam[4] = {eParamMix0,eParamMix1,eParamMix2,eParamMix3};
static const u8 timeParam[4] = {eParamTime0,eParamTime1,eParamTime2,eParamTime3};
static const u8 fbParam[4][4] = {
  {eParamFb00,eParamFb01,eParamFb02,eParamFb03},
  {eParamFb10,eParamFb11,eParamFb12,eParamFb13},
  {eParamFb20,eParamFb21,eParamFb22,eParamFb23},
  {eParamFb30,eParamFb31,eParamFb32,eParamFb33},
};

//======== local function proto
// static void processChannel(const fract32 *in, fract32 *out, const u8 chan);

//======== global function def
//----- init
void init_module(void) {
  u64 samp;
  u8 iChan, jChan;
  // clear the delay buffer
  for(samp=0; samp<ECHO_NUMSAMPS; samp++) {
    echoBuf[samp] = 0;
  }
  
  // init I/O pointers. FIXME: could eliminate by unrolling
  pOut[0] = &out0;
  pOut[1] = &out1;
  pOut[2] = &out2;
  pOut[3] = &out3;

  pIn[0] = &in0;
  pIn[1] = &in1;
  pIn[2] = &in2;
  pIn[3] = &in3;

  // intial values for parameters
  for(iChan=0; iChan<4; iChan++) {
    paramData[timeParam[iChan]].value.asFloat = 1.f * (f32)(iChan + 1);
    paramData[mixParam[iChan]].value.asFract = FRACT32_MAX * 0.5;
    for(jChan=0; jChan<4; jChan++) {
      if(iChan == jChan) {
	paramData[fbParam[iChan][jChan]].value.asFract = FRACT32_MAX * 0.75;
      } 
      else {
	paramData[fbParam[iChan][jChan]].value.asFract = 0;
      }
    }
  }
}

//------ process frame
void process_frame(void) {
  u8 i, j;
  fract32 wet;
  u64 echoFrames, rFrame;
  // FIXME: can unroll if necessary
  fract32 echo[4] = {0, 0, 0, 0};
  fract32 fb[4] = {0, 0, 0, 0};

  // loop channels for output
  for(i=0; i<4; i++) {

    // delay time in frames
    echoFrames = (u64)( paramData[timeParam[i]].value.asFloat 
			* (f32)ECHO_SAMPLERATE ) % ECHO_NUMFRAMES;
    
    if (wFrame < echoFrames) {
      rFrame = (wFrame + ECHO_NUMFRAMES) - echoFrames;
    }
    else {
      rFrame = wFrame - echoFrames;
    }
    
    wet = paramData[mixParam[i]].value.asFract;
    echo[i] = echoBuf[rFrame*4 + i];
    *(pOut[i]) = add_fr1x32(mult_fr1x32x32NS(wet, echo[i]),
			    mult_fr1x32x32NS(sub_fr1x32(FRACT32_MAX, wet), *(pIn[i]) ) );
   }

  // loop channels for buffer storage (2nd loop is necessary for cross-channel feedback)
  for(i=0; i<4; i++) {
    fb[i] = *(pIn[i]);
    for(j=0; j<4; j++) {
      fb[i] = add_fr1x32(fb[i], mult_fr1x32x32NS(echo[j], paramData[fbParam[i][j]].value.asFract));
    }
    echoBuf[wFrame*4 + i] = fb[i];
  }
 
  // increment and wrap echo write pointer
  wFrame++;
  if(wFrame > ECHO_NUMFRAMES_1) { wFrame = 0; }
}
