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
#include "filter_svf.h"
#include "delay.h"
#include "module.h"
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
// each input -> one svf
//static fract32 mix_adc_svf[4] = { 0, 0, 0, 0 };
// each svf -> one delay
//static fract32 mix_svf_del[4] = { 0, 0, 0, 0 };
// each delay -> one svf
//static fract32 mix_del_svf[4] = { 0, 0, 0, 0 };
// each svf -> each svf
/* static fract32 mix_svf_svf[4][4] = { { 0, 0, 0, 0 }, */
/* 				     { 0, 0, 0, 0 }, */
/* 				     { 0, 0, 0, 0 }, */
/* 				     { 0, 0, 0, 0 } }; */
// each delay -> each delay
fract32 mix_del_del[2][2] = { { 0, 0 }, { 0, 0 } };

// each input -> each output
fract32 mix_adc_dac[4][4] = { { 0, 0, 0, 0 },
				     { 0, 0, 0, 0 },
				     { 0, 0, 0, 0 },
				     { 0, 0, 0, 0 } };
// each delay -> each dac
fract32 mix_del_dac[2][4] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

// -- mixed inputs
// delay 
fract32 in_del[NLINES] = { 0, 0 };

//-- outputs
// delay
fract32 out_del[NLINES] = { 0, 0 };

#ifdef ARCH_BFIN // bfin
#else // linux : emulate bfin audio core
//fract32 in0, in1, in2, in3;
fract32 in[4];
#endif

//----------------------
//----- static functions

// mix delay inputs
static void mix_del_inputs(void) {
  //  u8 i, j;
  //  fract32* pIn;
  fract32 mul;
  
  //--- del 0
  in_del[0] = 0;
  // adc
  mul = mix_adc_del[0][0];
  /* if(mul != 0) { */ in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[0], mul)); /* } */
  mul = mix_adc_del[1][0];
  /* if(mul != 0) { */ in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[1], mul)); /* } */
  mul = mix_adc_del[2][0];
  /* if(mul != 0) { */ in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[2], mul)); /* } */
  mul = mix_adc_del[3][0];
  /* if(mul != 0) { */ in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(in[3], mul)); /* } */
  // del 
  mul = mix_del_del[0][0];
  /* if(mul != 0) { */ in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(out_del[0], mul)); /* } */
  mul = mix_del_del[1][0];
  /* if(mul != 0) { */ in_del[0] = add_fr1x32(in_del[0], mult_fr1x32x32(out_del[1], mul)); /* } */

  //--- del 1
  in_del[1] = 0;
  // adc
  mul = mix_adc_del[0][1];
  /* if(mul != 0) { */ in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[0], mul)); /* } */
  mul = mix_adc_del[1][1];
  /* if(mul != 0) { */ in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[1], mul)); /* } */
  mul = mix_adc_del[2][1];
  /* if(mul != 0) { */ in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[2], mul)); /* } */
  mul = mix_adc_del[3][1];
  /* if(mul != 0) { */ in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(in[3], mul)); /* } */
  // del 
  mul = mix_del_del[0][1];
  /* if(mul != 0) { */ in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(out_del[0], mul)); /* } */
  mul = mix_del_del[1][0];
  /* if(mul != 0) { */ in_del[1] = add_fr1x32(in_del[1], mult_fr1x32x32(out_del[1], mul)); /* } */
}


// mix svf inputs
/* static inline void mix_svf_inputs(void) { */
/*    u8 i, j; */
/*    fract32* pIn; */
/*    fract32 mul; */
/*   for(i=0; i<NLINES; i++) { */
/*     pIn = &(in_svf[i]); */
/*     // adc */
/*     mul = mix_adc_svf[i]; */
/*     if(mul == 0) { */
/*       *pIn = 0; */
/*     } else { */
/*       *pIn = mult_fr1x32x32(in[i], mul); */
/*     }  */
/*     // del */
/*     mul = mix_del_svf[i]; */
/*     if(mul == 0) { ;;  }  else { */
/*       *pIn = add_fr1x32(*pIn, mult_fr1x32x32(out_del[j], mul)); */
/*     } */
/*     for(j=0; j<NLINES; j++) { */
/*       // svf */
/*       mul = mix_svf_svf[j][i]; */
/*       if(mul == 0) { ;; } else { */
/* 	*pIn = add_fr1x32(*pIn, mult_fr1x32x32(out_svf[j], mul));       */
/*       } */
/*     } */
/*   } */
/* } */

// mix outputs
/* static void mix_outputs(void) { */
/*   u8 i, j; */
/*   fract32* pOut; */
/*   fract32 mul; */
/*   for(i=0; i<4; i++) { */
/*     pOut = &(out[i]); */
/*     *pOut = 0; */
/*     for(j=0; j<NLINES; j++) { */
/*       // del */
/*       mul = mix_del_dac[j][i]; */
/*       if(mul == 0 ) { ;; } else { */
/* 	*pOut = add_fr1x32(*pOut, mult_fr1x32x32(out_del[j], mul));       */
/*       } */
/*     } */
/*     for(j=0; j<4; j++) { */
/*       // adc */
/*       mul = mix_adc_dac[j][i]; */
/*       if(mul == 0) { ;; } else { */
/* 	*pOut = add_fr1x32(*pOut, mult_fr1x32x32(in[j], mul)); */
/*       } */
/*     } */
/*   } */
/* } */
static void mix_outputs(void) {
  fract32 mul;
  
  //-- out 0
  out[0] = 0;
  // del
  mul = mix_del_dac[0][0];
  /* if(mul != 0) { */ out[0] = add_fr1x32(out[0], mult_fr1x32x32(out_del[0], mul)); /* } */
  mul = mix_del_dac[1][0];
  /* if(mul != 0) { */ out[0] = add_fr1x32(out[0], mult_fr1x32x32(out_del[1], mul)); /* } */
  // adc
  mul = mix_adc_dac[0][0];
  /* if(mul != 0) { */ out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[0], mul)); /* } */
  mul = mix_adc_dac[1][0];
  /* if(mul != 0) { */ out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[1], mul)); /* } */
  mul = mix_adc_dac[2][0];
  /* if(mul != 0) { */ out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[2], mul)); /* } */
  mul = mix_adc_dac[3][0];
  /* if(mul != 0) { */ out[0] = add_fr1x32(out[0], mult_fr1x32x32(in[3], mul)); /* } */

  //-- out 1
  out[1] = 0;
  // del
  mul = mix_del_dac[0][1];
  /* if(mul != 0) { */ out[1] = add_fr1x32(out[1], mult_fr1x32x32(out_del[0], mul)); /* } */
  mul = mix_del_dac[1][1];
  /* if(mul != 0) { */ out[1] = add_fr1x32(out[1], mult_fr1x32x32(out_del[1], mul)); /* } */
  // adc
  mul = mix_adc_dac[0][1];
  /* if(mul != 0) { */ out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[0], mul)); /* } */
  mul = mix_adc_dac[1][1];
  /* if(mul != 0) { */ out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[1], mul)); /* } */
  mul = mix_adc_dac[2][1];
  /* if(mul != 0) { */ out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[2], mul)); /* } */
  mul = mix_adc_dac[3][1];
  /* if(mul != 0) { */ out[1] = add_fr1x32(out[1], mult_fr1x32x32(in[3], mul)); /* } */

  //-- out 2
  out[2] = 0;
  // del
  mul = mix_del_dac[0][2];
  /* if(mul != 0) { */ out[2] = add_fr1x32(out[2], mult_fr1x32x32(out_del[0], mul)); /* } */
  mul = mix_del_dac[1][2];
  /* if(mul != 0) { */ out[2] = add_fr1x32(out[2], mult_fr1x32x32(out_del[1], mul)); /* } */
  // adc
  mul = mix_adc_dac[0][2];
  /* if(mul != 0) { */ out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[0], mul)); /* } */
  mul = mix_adc_dac[1][2];
  /* if(mul != 0) { */ out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[1], mul)); /* } */
  mul = mix_adc_dac[2][2];
  /* if(mul != 0) { */ out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[2], mul)); /* } */
  mul = mix_adc_dac[3][2];
  /* if(mul != 0) { */ out[2] = add_fr1x32(out[2], mult_fr1x32x32(in[3], mul)); /* } */

  //-- out 3
  out[3] = 0;
  // del
  mul = mix_del_dac[0][3];
  /* if(mul != 0) { */ out[3] = add_fr1x32(out[3], mult_fr1x32x32(out_del[0], mul)); /* } */
  mul = mix_del_dac[1][3];
  /* if(mul != 0) { */ out[3] = add_fr1x32(out[3], mult_fr1x32x32(out_del[1], mul)); /* } */
  // adc
  mul = mix_adc_dac[0][3];
  /* if(mul != 0) { */ out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[0], mul)); /* } */
  mul = mix_adc_dac[1][3];
  /* if(mul != 0) { */ out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[1], mul)); /* } */
  mul = mix_adc_dac[2][3];
  /* if(mul != 0) { */ out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[2], mul)); /* } */
  mul = mix_adc_dac[3][3];
  /* if(mul != 0) { */ out[3] = add_fr1x32(out[3], mult_fr1x32x32(in[3], mul)); /* } */

}

//----------------------
//----- external functions

void module_init(void) {
  u8 i;
  // init module/param descriptor
#ifdef ARCH_BFIN 
  pLinesData = (linesData*)SDRAM_ADDRESS;
#else
  pLinesData = (linesData*)malloc(sizeof(linesData));
  // debug file
  //  dbgFile = fopen( "tape_dbg.txt", "w");
#endif
  
  gModuleData = &(pLinesData->super);
  gModuleData->paramDesc = (ParamDesc*)pLinesData->mParamDesc;
  gModuleData->paramData = (ParamData*)pLinesData->mParamData;
  gModuleData->numParams = eParamNumParams;

  fill_param_desc();
  
  /// test
  // return;
  /////
  
  for(i=0; i<NLINES; i++) {
    delay_init(&(lines[i]), pLinesData->audioBuffer[i], LINES_BUF_FRAMES);
    filter_svf_init(&(svf[i]));
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
  u8 i;

  /// test
  //  return;
  ////

  mix_del_inputs();
  //  mix_svf_inputs();

  //  in_del[0] = in[0];
  //  in_del[1] = in[1];

  out_del[0] = filter_svf_next( &(svf[0]), delay_next( &(lines[0]), in_del[0]) );
  out_del[1] = filter_svf_next( &(svf[1]), delay_next( &(lines[1]), in_del[1]) );

  //  for(i=0; i<NLINES; i++) {
  //    out_del[i] = delay_next( &(lines[i]), in_del[i] );
    //    out_del[i] = filter_svf_next( &(svf[i]), out_del[i] );
    //    out_svf[i] = filter_svf_next( &(svf[i]), in_svf[i] );
    //  }

  //  out_del[0] = delay_next( &(lines[0]), in_del[0] );
  //  out_del[1] = delay_next( &(lines[1]), in_del[1] );
  
  mix_outputs();
  //  out[0] = in[0];
  //  out[0] = mult_fr1x32x32(in[0], mix_adc_dac[0][0]);
  //  out[1] = mult_fr1x32x32(in[1], mix_adc_dac[1][1]);
  //  out[2] = mult_fr1x32x32(in[2], mix_adc_dac[2][2]);
  //  out[3] = mult_fr1x32x32(in[3], mix_adc_dac[3][3]);

  //  out[0] = add_fr1x32(in[0], out_del[0] );
  //  out[1] = add_fr1x32(in[1], out_del[1] );

}


// parameter set function
// (a very long switch statement!)
#include "param_set.c"
