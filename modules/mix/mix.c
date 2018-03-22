/* 
   mix.c
   aleph-bfin

   a very simple template module.

   applies attenuation with slew to each input signal,

   then mixes all attenuated signals to all outputs.

   also performs CV changes with slew.

*/

//-- aleph/common headers
#include "types.h"
#include <string.h>

//-- aleph/bfin-lib headers
// global variables
#include "bfin_core.h"
// cv output driver
#include "cv.h"
// gpio pin numbers
#include "gpio.h"
#include "fract_math.h"

//-- dsp class headers
// simple 1-pole integrator
#include "filter_1p.h"
// global declarations for module data
#include "module.h"

//--- custom headers
// parameter lists and constants
#include "params.h"

#define BUFFERSIZE 0x100000

typedef struct _sampleBuffer {
    volatile fract32 *data;             //pointer to data
    u32 samples;                        //count of samples
} sampleBuffer;

typedef struct _bufferHead {
    sampleBuffer *buf;                  //pointer to buffer
    u32 idx;                            //current idx
} bufferHead;

static void buffer_init(sampleBuffer *buf, volatile fract32 *data, u32 samples);
static void buffer_head_init(bufferHead *head, sampleBuffer *buf);
static s32 buffer_head_play(bufferHead *head);

typedef struct _mixData {
    ModuleData super;
    ParamData mParamData[eParamNumParams];
    
    volatile fract32 sampleBuffer[BUFFERSIZE];
} mixData;

ModuleData *gModuleData;
mixData *data;
sampleBuffer onchipbuffer[4];
u32 offset = 0;

bufferHead heads[4];

fract32 adcVal[4] = { 0, 0, 0, 0 };
filter_1p_lo adcSlew[4];
fract32 cvVal[4] = { 0, 0, 0, 0 };
filter_1p_lo cvSlew[4];
u8 cvChan = 0;
fract32 outBus = 0;

static void process_cv(void);
static inline void param_setup(u32 id, ParamValue v) {
    gModuleData->paramData[id].value = v;
    module_set_param(id, v);
}

void module_init(void) {
    data = (mixData*)SDRAM_ADDRESS;
    gModuleData = &(data->super);
    strcpy(gModuleData->name, "mix");
    gModuleData->paramData = data->mParamData;
    gModuleData->numParams = eParamNumParams;

    u32 n, i;

    for(n=0; n<4; n++)
    {
        buffer_init(&(onchipbuffer[n]), data->sampleBuffer, BUFFERSIZE);
    }
    
    for(n=0; n<4; n++)
    {
        buffer_head_init(&(heads[n]), &(onchipbuffer[n]));
    }

    for(i=0; i<BUFFERSIZE; i++)
    {
        data->sampleBuffer[i] = 0x00000000;
    }

    offset = 0;
    
    //  init parameters
//    for(n=0; n<module_get_num_params(); n++) param_setup(n, 0);}

    // initialize 1pole filters for input attenuation slew
    filter_1p_lo_init( &(adcSlew[0]), 0 );
    filter_1p_lo_init( &(adcSlew[1]), 0 );
    filter_1p_lo_init( &(adcSlew[2]), 0 );
    filter_1p_lo_init( &(adcSlew[3]), 0 );

    // initialize 1pole filters for cv output slew
    filter_1p_lo_init( &(cvSlew[0]), 0 );
    filter_1p_lo_init( &(cvSlew[1]), 0 );
    filter_1p_lo_init( &(cvSlew[2]), 0 );
    filter_1p_lo_init( &(cvSlew[3]), 0 );

    // set initial param values
    // constants are from params.h
    param_setup(eParam_cv0, 0 );
    param_setup(eParam_cv1, 0 );
    param_setup(eParam_cv2, 0 );
    param_setup(eParam_cv3, 0 );

    // set amp to 1/4 (-12db) with right-shift intrinsic
    param_setup(eParam_adc0, PARAM_AMP_MAX >> 2 );
    param_setup(eParam_adc1, PARAM_AMP_MAX >> 2 );
    param_setup(eParam_adc2, PARAM_AMP_MAX >> 2 );
    param_setup(eParam_adc3, PARAM_AMP_MAX >> 2 );

    // set slew defaults. the value is pretty arbitrary
    param_setup(eParam_adcSlew0, PARAM_SLEW_DEFAULT);
    param_setup(eParam_adcSlew1, PARAM_SLEW_DEFAULT);
    param_setup(eParam_adcSlew2, PARAM_SLEW_DEFAULT);
    param_setup(eParam_adcSlew3, PARAM_SLEW_DEFAULT);
    param_setup(eParam_cvSlew0, PARAM_SLEW_DEFAULT);
    param_setup(eParam_cvSlew1, PARAM_SLEW_DEFAULT);
    param_setup(eParam_cvSlew2, PARAM_SLEW_DEFAULT);
    param_setup(eParam_cvSlew3, PARAM_SLEW_DEFAULT);
}

// de-init (never actually used on blackfin, but maybe by emulator)
void module_deinit(void) {
  ;;
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}



// frame process function! 
// called each audio frame from codec interrupt handler
// ( bad, i know, see github issues list )
void module_process_frame(void) {
    heads[0].idx += 1;
    if (heads[0].idx > 48000)
    {
        heads[0].idx = 0;
    }

    out[0] = out[1] = buffer_head_play(&(heads[0]));

    /*
    const fract32 c = 1664525 ;
    const fract32 a = 1013904223 ;
    static fract32 x = 666;
    x = x * c + a;
    */
}

// sample offset
void module_set_offset(ParamValue v) {
    offset = v;
}

// sample
void module_set_sample(ParamValue v) {
    data->sampleBuffer[offset] = v;
    offset++;
}

// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  // switch on the param index
  switch(idx) {
    // cv output values
  case eParam_cv0 :
    filter_1p_lo_in( &(cvSlew[0]), v );
    break;
  case eParam_cv1 :
    filter_1p_lo_in( &(cvSlew[1]), v );
    break;
  case eParam_cv2 :
    filter_1p_lo_in( &(cvSlew[2]), v );
    break;
  case eParam_cv3 :
    filter_1p_lo_in( &(cvSlew[3]), v );

    // cv slew values
    break;
  case eParam_cvSlew0 :
   filter_1p_lo_set_slew(&(cvSlew[0]), v);
    break;
  case eParam_cvSlew1 :
    filter_1p_lo_set_slew(&(cvSlew[1]), v);
    break;
  case eParam_cvSlew2 :
    filter_1p_lo_set_slew(&(cvSlew[2]), v);
    break;
  case eParam_cvSlew3 :
    filter_1p_lo_set_slew(&(cvSlew[3]), v);
    break;

    // input attenuation values
  case eParam_adc0 :
    filter_1p_lo_in( &(adcSlew[0]), v );
    break;
  case eParam_adc1 :
    filter_1p_lo_in( &(adcSlew[1]), v );
    break;
  case eParam_adc2 :
    filter_1p_lo_in( &(adcSlew[2]), v );
    break;
  case eParam_adc3 :
    filter_1p_lo_in( &(adcSlew[3]), v );

    // input attenuation slew values
    break;
  case eParam_adcSlew0 :
   filter_1p_lo_set_slew(&(adcSlew[0]), v);
    break;
  case eParam_adcSlew1 :
    filter_1p_lo_set_slew(&(adcSlew[1]), v);
    break;
  case eParam_adcSlew2 :
    filter_1p_lo_set_slew(&(adcSlew[2]), v);
    break;
  case eParam_adcSlew3 :
    filter_1p_lo_set_slew(&(adcSlew[3]), v);
    break;

  default:
    break;
  }
}

//----- static function definitions

//// NOTE / FIXME:
/* CV updates are staggered, each channel on a separate audio frame. 
   
   the reason for this is that each channel takes some time to update.
   for now, we just wait a full frame between channels,
   and effectively reduce CV output sampling rate by a factor of 4.
   (as well as changing the effecticve slew time, which is  not great.)

   the more proper way to do this would be:
   - enable DMA tx interrupt
   - each ISR calls the next channel to be loaded
   - last thing audio ISR does is call the first DAC channel to be loaded
   - cv_update writes to 4x16 volatile buffer

   this could give rise to its own problems:
   audio frame processing is currently interrupt-driven per frame,
   so CV tx interrupt could be masked by the next audio frame if the schedule is tight.
*/
// update cv output
void process_cv(void) {
  // process the current channel
  // do nothing if the value is stable
  if( filter_1p_sync( &(cvSlew[cvChan]) ) ) {
    ;;
  } else {
    // update the slew filter and store the value
      cvVal[cvChan] = filter_1p_lo_next(&(cvSlew[cvChan]));
      // send the value to the cv driver
      cv_update( cvChan, cvVal[cvChan] );
  }

  // update the channel to be processed
  if(++cvChan == 4) {
    cvChan = 0;
  }
}

void buffer_init(sampleBuffer *buf, volatile fract32 *data, u32 samples) {
    buf->data = data;
    buf->samples = samples;
}

void buffer_head_init(bufferHead *head, sampleBuffer *buf) {
    head->buf = buf;
    head->idx = 0;
}

s32 buffer_head_play(bufferHead *head) {
    return head->buf->data[head->idx];
}
