/* lines.c
   aleph-audio

   multichannel, routable delay lines + filters.
   suitable for echoes, loopers, etc
*/

// std
#include <math.h>
// (testing)
#include <stdlib.h>
#include <string.h>

// aleph-common
#include "fix.h"
#include "types.h"


#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

// audio
#include "buffer.h"
#include "filter_svf.h"
#include "delay.h"
#include "module.h"
////test
#include "noise.h"
/// lines
#include "params.h"

//-----------------------
//------ static variables
#if ARCH_LINUX
static FILE* dbgFile;
u8 dbgFlag = 0;
u32 dbgCount = 0;
#endif

// total SDRAM is 64M
// 60 seconds
#define LINES_BUF_FRAMES 0x2bf200
#define NLINES 2

//---- param ranges

// rate
#define RATE_MIN 0x2000 // 1/8
#define RATE_MAX 0x80000 // 8

// time
#define TIME_MIN 0
//  seconds in 16.16x
#define TIME_MAX 0x3c

// svf cutoff
#define SVF_HZ_MIN 0x200000      // 32
#define SVF_HZ_MAX 0x40000000    // 16384


// data structure of external memory
typedef struct _linesData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
  fract32 audioBuffer[NLINES][LINES_BUF_FRAMES];
} linesData;

//-------------------------
//----- extern vars (initialized here)
moduleData* gModuleData; 


//-----------------------
//------ static variables

// pointer to all external memory
linesData* pLinesData;

// delay lines (each has buffer descriptor and read/write taps)
delayLine lines[NLINES];

// state variable filters
filter_svf svf[NLINES];

//---- mix points
// each input -> one delay
fract32 mix_adc_del[4][2] = { {0, 0}, {0, 0}, {0, 0}, {0, 0} };
// each delay -> each delay
fract32 mix_del_del[2][2] = { { 0, 0 }, { 0, 0 } };
// each input -> each output
fract32 mix_adc_dac[4][4] = { { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 },
			      { 0, 0, 0, 0 } };
// each delay -> each dac
fract32 mix_del_dac[2][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

// svf balance
fract32 mix_fwet[NLINES] = { 0, 0 };
fract32 mix_fdry[NLINES] = { 0, 0 };

// -- mixed inputs
// delay 
fract32 in_del[NLINES] = { 0, 0 };

//-- outputs
// delay
fract32 out_del[NLINES] = { 0, 0 };
fract32 out_svf[NLINES] = { 0, 0 };


///////////////
/////////////////

// mix delay inputs
static void mix_del_inputs(void) {
  //  u8 i, j;
  //  fract32* pIn;
  fract32 mul;
  
  //--- del 0
  in_del[0] = 0;

  mul = mix_adc_del[0][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[0], mul)); 
  mul = mix_adc_del[1][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[1], mul)); 
  mul = mix_adc_del[2][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[2], mul)); 
  mul = mix_adc_del[3][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[3], mul)); 

  // del 
  mul = mix_del_del[0][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(out_del[0], mul)); 
  mul = mix_del_del[1][0];
  in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(out_del[1], mul)); 

  //--- del 1
  in_del[1] = 0;
  // adc
  mul = mix_adc_del[0][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[0], mul)); 
  mul = mix_adc_del[1][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[1], mul)); 
  mul = mix_adc_del[2][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[2], mul)); 
  mul = mix_adc_del[3][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[3], mul)); 
  // del 
  mul = mix_del_del[0][1];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(out_del[0], mul)); 
  mul = mix_del_del[1][0];
  in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(out_del[1], mul));/*  }*/
}

  static void mix_outputs(void) {
    fract32 mul;
  
    //-- out 0
    out[0] = 0;
    // del
    mul = mix_del_dac[0][0];
    /*  if(mul != 0) { */ out[0] = add_fr1x32(out[0], mult_fr1x32x32(out_del[0], mul)); 
    mul = mix_del_dac[1][0];
    out[0] = add_fr1x32(out[0], mult_fr1x32x32(out_del[1], mul)); 
    // adc
    mul = mix_adc_dac[0][0];
    out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[0], mul)); 
    mul = mix_adc_dac[1][0];
    out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[1], mul)); 
    mul = mix_adc_dac[2][0];
    out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[2], mul)); 
    mul = mix_adc_dac[3][0];
    out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[3], mul)); 

    //-- out 1
    out[1] = 0;
    // del
    mul = mix_del_dac[0][1];
    out[1] = add_fr1x32(out[1], mult_fr1x32x32(out_del[0], mul)); 
    mul = mix_del_dac[1][1];
    out[1] = add_fr1x32(out[1], mult_fr1x32x32(out_del[1], mul)); 
    // adc
    mul = mix_adc_dac[0][1];
    out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[0], mul)); 
    mul = mix_adc_dac[1][1];
    out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[1], mul)); 
    mul = mix_adc_dac[2][1];
    out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[2], mul)); 
    mul = mix_adc_dac[3][1];
    out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[3], mul)); 

    //-- out 2
    out[2] = 0;
    // del
    mul = mix_del_dac[0][2];
    out[2] = add_fr1x32(out[2], mult_fr1x32x32(out_del[0], mul)); 
    mul = mix_del_dac[1][2];
    out[2] = add_fr1x32(out[2], mult_fr1x32x32(out_del[1], mul)); 
    // adc
    mul = mix_adc_dac[0][2];
    out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[0], mul)); 
    mul = mix_adc_dac[1][2];
    out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[1], mul)); 
    mul = mix_adc_dac[2][2];
    out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[2], mul)); 
    mul = mix_adc_dac[3][2];
    out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[3], mul)); 

    //-- out 3
    out[3] = 0;
    // del
    mul = mix_del_dac[0][3];
    out[3] = add_fr1x32(out[3], mult_fr1x32x32(out_del[0], mul)); 
    mul = mix_del_dac[1][3];
    out[3] = add_fr1x32(out[3], mult_fr1x32x32(out_del[1], mul)); 
    // adc
    mul = mix_adc_dac[0][3];
    out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[0], mul)); 
    mul = mix_adc_dac[1][3];
    out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[1], mul)); 
    mul = mix_adc_dac[2][3];
    out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[2], mul)); 
    mul = mix_adc_dac[3][3];
    out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[3], mul));

  }

  //----------------------
  //----- external functions

  void module_init(void) {
    u8 i;
    u32 j;
    // init module/param descriptor
#ifdef ARCH_BFIN 
    pLinesData = (linesData*)SDRAM_ADDRESS;
#else
    pLinesData = (linesData*)malloc(sizeof(linesData));
    // debug file
    //  dbgFile = fopen( "tape_dbg.txt", "w");
#endif
  
    gModuleData = &(pLinesData->super);
    strcpy(gModuleData->name, "aleph-lines");

    gModuleData->paramDesc = (ParamDesc*)pLinesData->mParamDesc;
    gModuleData->paramData = (ParamData*)pLinesData->mParamData;
    gModuleData->numParams = eParamNumParams;
    fill_param_desc();
  
    for(i=0; i<NLINES; i++) {
      delay_init(&(lines[i]), pLinesData->audioBuffer[i], LINES_BUF_FRAMES);
      filter_svf_init(&(svf[i]));
    
      filter_svf_set_rq(&(svf[i]), 0x1000);
      filter_svf_set_low(&(svf[i]), 0x4000);
    
      for(j=0; j<LINES_BUF_FRAMES; j++) {
	pLinesData->audioBuffer[i][j] = 0;
      }
    }
  }

  // de-init
  void module_deinit(void) {
  }

  // get number of parameters
  u32 module_get_num_params(void) {
    return eParamNumParams;
  }


  void module_process_frame(void) { 
    static fract32 tmpDel, tmpSvf;
    u8 i;

    tmpDel = 0;
    tmpSvf = 0;

    // mix inputs to delay lines
    mix_del_inputs();

    for(i=0; i<NLINES; i++) {
      // process delay line
      tmpDel = delay_next( &(lines[i]), in_del[i]);	    
      // process filter
      tmpSvf = filter_svf_next( &(svf[i]), tmpDel);  
      // mix
      tmpDel = mult_fr1x32x32( tmpDel, mix_fdry[i] );
      tmpDel = add_fr1x32(tmpDel, mult_fr1x32x32(tmpSvf, mix_fwet[i]) );
      out_del[i] = tmpDel;
      // hard patching always works...
      //      out_del[i] = out_svf[i];

    } // end lines loop 
  
    /* out_svf[0] = filter_svf_next(&(svf[0]), noise_next()); */
    /* out_del[0] = out_svf[0]; */
    /* out_del[1] = 0; */

    // mix outputs to DACs
    mix_outputs();
  }


  // parameter set function
  // (a very long switch statement)
#include "param_set.c"

