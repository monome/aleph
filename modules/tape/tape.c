/* tape.c
   aleph-bfin

   Analogue Tape emulator (varispeed read/write)
*/

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "types.h"

// aleph-bfin
#include "bfin_core.h"
#include "cv.h"
#include "gpio.h"
#include "fract_math.h"

// audio
#include "filter_1p.h"
#include "module.h"
#include "buffer16.h"
#include "pan.h"

/// custom
#include "params.h"

// data structure of external memory
typedef struct _tapeData {
  ModuleData super;
  ParamData mParamData[eParamNumParams];
  volatile fract16 audioBuffer[0x10000];
} tapeData;

//-------------------------
//----- extern vars (initialized here)
ModuleData* gModuleData;

//-----------------------
//------ static variables

// pointer to all external memory
tapeData* pDacsData;

// dac values (u16, but use fract32 and audio integrators)
static fract32 cvVal[4];
static filter_1p_lo cvSlew[4];
static u8 cvChan = 0;

//----------------------
//----- external functions

static inline void param_setup(u32 id, ParamValue v);
static inline void param_setup(u32 id, ParamValue v) {
  gModuleData->paramData[id].value = v;
  module_set_param(id, v);
}

audioBuffer16 tape;
buffer16Tap24_8 wr;
buffer16TapN wrN;
buffer16Tap24_8 rd;

void module_init(void) {
  // init module/param descriptor
  pDacsData = (tapeData*)SDRAM_ADDRESS;

  gModuleData = &(pDacsData->super);
  strcpy(gModuleData->name, "tape");

  gModuleData->paramData = (ParamData*)pDacsData->mParamData;
  gModuleData->numParams = eParamNumParams;

  filter_1p_lo_init( &(cvSlew[0]), 0 );
  filter_1p_lo_init( &(cvSlew[1]), 0 );
  filter_1p_lo_init( &(cvSlew[2]), 0 );
  filter_1p_lo_init( &(cvSlew[3]), 0 );

  param_setup( eParam_cvSlew0, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew1, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew2, PARAM_CV_SLEW_DEFAULT );
  param_setup( eParam_cvSlew3, PARAM_CV_SLEW_DEFAULT );

  param_setup( eParam_cvVal0, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal1, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal2, PARAM_CV_VAL_DEFAULT );
  param_setup( eParam_cvVal3, PARAM_CV_VAL_DEFAULT );
  buffer16_init(&tape, pDacsData->audioBuffer, 0x10000);
  buffer16Tap24_8_init(&wr, &tape);
  buffer16_tapN_init(&wrN, &tape);
  buffer16Tap24_8_init(&rd, &tape);
  fract16 writeSpeed = -90;
  fract16 readSpeed = 256 ;
  buffer16Tap24_8_set_rate(&wr, writeSpeed);
  buffer16Tap24_8_set_rate(&rd, readSpeed);
  buffer16Tap24_8_set_loop(&wr, 10 * 4800);
  buffer16Tap24_8_set_loop(&rd, 10 * 4800);
  wrN.loop =  10 * 4800;
#ifdef ARCH_LINUX
  int i;
  FILE *f;
  double in;
  f = fopen("pan_test.csv", "w");
  if (f) {
    for(i=0; i < FR16_MAX; i++) {
      fprintf(f, "%d %d\n", i, pan_lin_mix16(0, FR16_MAX >> 2, i));
    }
    fclose (f);
  }

  // Test Readhead linear interpolation
  for(i=0; i < 4800; i++) {
    in = FR16_MAX * sin(2 * M_PI / 48.0 * i);
    pDacsData->audioBuffer[i] = (fract16) in;
  }
  f = fopen("interp_test_read.csv", "w");
  if(f) {
    for(i=0; i < 4800; i++) {
      in = FR16_MAX * sin(2 * M_PI / 48.0 * i);
      buffer16Tap24_8_next(&rd);
      fprintf(f, "%f %d %f %d\n", (float) i, (int) in, ((float) rd.idx) / 256.0, buffer16Tap24_8_read(&rd));
    }
    fclose(f);
  }

  // Test Writehead linear interpolation
  // first zero the buffer (debug)
  for(i=0; i < 4800; i++) {
    pDacsData->audioBuffer[i] = 0;
  }
  f = fopen("interp_test_write.csv", "w");
  if(f) {
    double writePositions[48000];
    fract16 writtenData[48000];
    for(i=0; i < 4800; i++) {
      writtenData[i] = FR16_MAX * sin(2 * M_PI / 48.0 * i);
      buffer16Tap24_8_next(&wr);
      writePositions[i] = ((float) wr.idx) / 256.0;
      buffer16Tap24_8_write(&wr, writtenData[i]);
    }
    for(i=4800; i < 48000; i++) {
      writtenData[i] = 0;
      writePositions[i] = ((float) wr.idx) / 256.0;
    }
    for(i=0; i < 48000; i++) {
      fprintf(f, "%f %d %f %d\n",
	      writePositions[i], writtenData[i],
	      (float) i, pDacsData->audioBuffer[i]);
    }
    fclose(f);
  }

#endif
}

// de-init
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
  return eParamNumParams;
}

//// FIXME:
/* for now, stagger DAC channels across consecutive audio frames
   better method might be:
   - enable DMA tx interrupt
   - each ISR calls the next channel to be loaded
   - last thing audio ISR does is call the first DAC channel to be loaded
   - dac_update writes to 4x16 volatile buffer
*/

void module_process_frame(void) {

  // Update one of the CV outputs
  if(filter_1p_sync(&(cvSlew[cvChan]))) { ;; } else {
    cvVal[cvChan] = filter_1p_lo_next(&(cvSlew[cvChan]));
    cv_update(cvChan, cvVal[cvChan]);
  }

  // Queue up the next CV output for processing next audio frame
  if(++cvChan == 4) {
    cvChan = 0;
  }

  buffer16Tap24_8_next(&wr);
  buffer16Tap24_8_next(&rd);
  buffer16Tap24_8_write(&wr, in[0] >> 16);
  out[0] = buffer16Tap24_8_read(&rd) << 16;

  /* buffer16_tapN_next(&wrN); */
  /* buffer16Tap24_8_next(&rd); */
  /* buffer16_tapN_write(&wrN, in[0] >> 16); */
  /* out[0] = buffer16Tap24_8_read(&rd) << 16; */

  /* out[0] = 0; */
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
}

// parameter set function
void module_set_param(u32 idx, ParamValue v) {
  LED4_TOGGLE; // which one it this?
  switch(idx) {

  case eParam_cvVal0 :
    filter_1p_lo_in(&(cvSlew[0]), v);
    break;
  case eParam_cvVal1 :
    filter_1p_lo_in(&(cvSlew[1]), v);
    break;
  case eParam_cvVal2 :
    filter_1p_lo_in(&(cvSlew[2]), v);
    break;
  case eParam_cvVal3 :
    filter_1p_lo_in(&(cvSlew[3]), v);
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
  default:
    break;
  }
}
