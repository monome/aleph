/* 
   mix.c
   aleph-bfin

   a very simple template module.

   applies attenuation with slew to each input signal,

   then mixes all attenuated signals to all outputs.

   also performs CV changes with slew.

*/

//-- blackfin toolchain headers
// 1.32 and 1.15 fixed-point arithmeti
#include "fract_math.h"

//-- aleph/common headers
#include "types.h"

//-- aleph/bfin-lib headers
// global variables
#include "bfin_core.h"
// cv output driver
#include "cv.h"
// gpio pin numbers
#include "gpio.h"

//-- dsp class headers
// simple 1-pole integrator
#include "filter_1p.h"
// global declarations for module data
#include "module.h"

//--- custom headers
// parameter lists and constants
#include "params.h"

// customized module data structure
// this will be located at the top of SDRAM (64MB)
// all other variables (code, stack, heap) are located in SRAM (64K)
typedef struct _mixData {
  //... here is where you would put other large data structures.

  // for example, a multichannel delay module would need an audio buffer:
  //  volatile fract32 audioBuffer[NUM_BUFS][FRAMES_PER_BUF];

  // bear in mind that access to SDRAM is significantly slower than SRAM (10-20 cycles!)
  // so don't put anything here that doesn't need the extra space.
} mixData;

//-------------------------
//----- extern variables (initialized here)


// global pointer to module descriptor.
// required by the aleph-bfin library!
ModuleData* gModuleData;

//-----------------------bfin_lib/src/
//------ static variables

/* 
   here's the actual memory for module descriptor and param data
   global pointers are to point at these here during module init.
   we do it in this indirect way, because
   a) modules have variable param count
   b) in an extreme case, might need to locate param data in SDRAM
      ( until recently, SDRAM contained full param descriptors.)
*/
static ModuleData super;
static ParamData mParamData[eParamNumParams];

// input values
static fract32 adcVal[4];
static filter_1p_lo adcSlew[4];


// cv values (16 bits, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
// current channel to update - see below in process_cv() 
static u8 cvChan = 0;

// audio output bus
static fract32 outBus = 0;

//-----------------
//--- static function declaration

// update cv output
static void process_cv(void);

// small helper function to set parameter initial values
static inline void param_setup(u32 id, ParamValue v) {
  // set the input data so that bfin core will report it
  // back to the controller via SPI, when requested. 
  // (bees will make such a request of each param at launch to sync with network.)
  // this is also how a polled analysis parameter would work.
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}


//----------------------
//----- external functions

void module_init(void) {
  // initialize module superclass data
  // by setting global pointers to our own data
  gModuleData = &super;
  gModuleData->paramData = mParamData;
  gModuleData->numParams = eParamNumParams;

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

  //--- process slew

  // 
  // update filters, calling "class method" on pointer to each
  adcVal[0] = filter_1p_lo_next( &(adcSlew[0]) );
  adcVal[1] = filter_1p_lo_next( &(adcSlew[1]) );
  adcVal[2] = filter_1p_lo_next( &(adcSlew[2]) );
  adcVal[3] = filter_1p_lo_next( &(adcSlew[3]) );

  //--- mix

  // zero the output bus
  outBus = 0;

  /* 
     note the use of fract32 arithmetic intrinsics!
     these are fast saturating multiplies/adds for 32bit signed fractions in [-1, 1)
     there are also intrinsics for fr16, mixed modes, and conversions.
     for details see:
     http://blackfin.uclinux.org/doku.php?id=toolchain:built-in_functions
  */
  // scale each input and add it to the bus
  outBus = add_fr1x32( outBus, mult_fr1x32x32(in[0], adcVal[0]) );
  outBus = add_fr1x32( outBus, mult_fr1x32x32(in[1], adcVal[1]) );
  outBus = add_fr1x32( outBus, mult_fr1x32x32(in[2], adcVal[2]) );
  outBus = add_fr1x32( outBus, mult_fr1x32x32(in[3], adcVal[3]) );

  // copy the bus to all the outputs
  out[0] = outBus;
  out[1] = outBus;
  out[2] = outBus;
  out[3] = outBus;


  //--- cv
  process_cv();
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
