#include <fract_math.h>
#include "bfin_core.h"
#include "param.h"
#include "module.h"
#include "echo_data.h"

// forward declaration from echo_data.c
// (this seems to be necessary for the linker script to work)
//extern fract32 echoBuf[];

//------- global variable definitions
ParamData paramData[MODULE_NUM_PARAMS];

//-------- local variables
// next frame index to write in delay buffer.
static u64 wFrame;

//------ local function prototypes
static void processChannel(const fract32 *in,
			   fract32 *out,
			   const u8 chan,
			   u64 *rFrame,
			   fract32* wetMix, 
			   fract32* dryMix);

//-------- global function definitions
void init_module(void) {
  u64 i;
  // clear the delay buffer
  for(i=0; i<ECHO_NUMFRAMES * 4; i++) {
    echoBuf[i] = 0;
  }
  // intialize param values
  // echo time (float):
  paramData[0].value.f = 1.f;
  // mix:
  paramData[1].value.si = FRACT32_MAX * 0.5;
  // feedback:
  paramData[2].value.si = FRACT32_MAX * 0.75;
}

void process_frame(void) {
  // local copies of the appropriate region of delay buffer data
  // (it seems like it will be faster to grab these all in a single linear fetch)
  // for now, we are not interpolating, so we only need 1 read frame * 4 channels.
  //  fract32 buf00, buf01, buf10, buf11;
  // read frame
  u64 rFrame;
  const u64 echoFrames = ((u64)(paramData[0].value.f * (f32)ECHO_SAMPLERATE)) % ECHO_NUMFRAMES;
  // intermediate params
  fract32 wetMix, dryMix;
 
  if (wFrame < echoFrames) {
    rFrame = wFrame + ECHO_NUMFRAMES - echoFrames;
  } else {
    rFrame = wFrame - echoFrames;
  }

  wetMix = paramData[1].value.si;
  dryMix = sub_fr1x32(FRACT32_MAX, wetMix);

// TODO: optimize stack usage here, this is pretty dumb
  processChannel(&in00, &out00, 0, &rFrame, &wetMix, &dryMix);
  processChannel(&in01, &out01, 1, &rFrame, &wetMix, &dryMix);
  processChannel(&in10, &out10, 2, &rFrame, &wetMix, &dryMix);
  processChannel(&in11, &out11, 3, &rFrame, &wetMix, &dryMix);
}

//------- local function definitions
static void processChannel(const fract32 *in,
			   fract32 *out,
			   const u8 chan,
			   u64 *rFrame,
			   fract32* wetMix, 
			   fract32* dryMix) {
			   
  const fract32 echo = echoBuf[(*rFrame) * 4 + chan];
  // output: input mixed with contents of delay buffer
  *out = add_fr1x32(mult_fr1x32x32NS(echo, *wetMix),
		    mult_fr1x32x32NS(*in, *dryMix)
		    );
  // store: input mixed with output*fb
  echoBuf[wFrame*4 + chan] = add_fr1x32(*in, mult_fr1x32x32NS(echo, paramData[2].value.si));
}
