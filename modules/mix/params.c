/*

  params.c
  
  defines the module parameter descriptor fields.
   
  this is done in the descriptor helper program, 
  *not* in the module code itself!

  */

#include <string.h>
#include "module.h"
#include "params.h" 

// this function
extern void fill_param_desc(ParamDesc* desc) {
  
  // obviously, these could be done in other ways ( easier to read/edit maybe. )
  // but neither code space nor speed is a big deal here.
  // this code can also be script-generated easily enough.

  /// input values have param type "Amp"
  // they range from 0 to the full positive value of fract32
  /// and will be displayed as dB by bees.
  strcpy(desc[eParam_adc0].label, "adc0");
  desc[eParam_adc0].type = eParamTypeAmp; 
  desc[eParam_adc0].min = 0x00000000; 
  desc[eParam_adc0].max = PARAM_AMP_MAX;
  desc[eParam_adc0].radix = 16; // doesn't matter

  strcpy(desc[eParam_adc1].label, "adc1");
  desc[eParam_adc1].type = eParamTypeAmp; 
  desc[eParam_adc1].min = 0x00000000; 
  desc[eParam_adc1].max = PARAM_AMP_MAX;
  desc[eParam_adc1].radix = 16; // doesn't matter

  strcpy(desc[eParam_adc2].label, "adc2");
  desc[eParam_adc2].type = eParamTypeAmp; 
  desc[eParam_adc2].min = 0x00000000; 
  desc[eParam_adc2].max = PARAM_AMP_MAX;
  desc[eParam_adc2].radix = 16; // doesn't matter

  strcpy(desc[eParam_adc3].label, "adc3");
  desc[eParam_adc3].type = eParamTypeAmp; 
  desc[eParam_adc3].min = 0x00000000; 
  desc[eParam_adc3].max = PARAM_AMP_MAX;
  desc[eParam_adc3].radix = 16; // doesn't matter

  /// slew values have param type "Integrator"
  // they range from 0 to the full positive value of fract32
  /// and will be displayed as seconds-to-convergence by bees.

  strcpy(desc[eParam_adcSlew0].label, "adcSlew0");
  desc[eParam_adcSlew0].type = eParamTypeIntegrator; 
  desc[eParam_adcSlew0].min = 0x00000000; 
  desc[eParam_adcSlew0].max = PARAM_SLEW_MAX;
  desc[eParam_adcSlew0].radix = 16; // doesn't matter

  strcpy(desc[eParam_adcSlew1].label, "adcSlew1");
  desc[eParam_adcSlew1].type = eParamTypeIntegrator; 
  desc[eParam_adcSlew1].min = 0x00000000; 
  desc[eParam_adcSlew1].max = PARAM_SLEW_MAX;
  desc[eParam_adcSlew1].radix = 16; // doesn't matter

  strcpy(desc[eParam_adcSlew2].label, "adcSlew2");
  desc[eParam_adcSlew2].type = eParamTypeIntegrator; 
  desc[eParam_adcSlew2].min = 0x00000000; 
  desc[eParam_adcSlew2].max = PARAM_SLEW_MAX; 
  desc[eParam_adcSlew2].radix = 16; // doesn't matter

  strcpy(desc[eParam_adcSlew3].label, "adcSlew3");
  desc[eParam_adcSlew3].type = eParamTypeIntegrator; 
  desc[eParam_adcSlew3].min = 0x00000000; 
  desc[eParam_adcSlew3].max = PARAM_SLEW_MAX;
  desc[eParam_adcSlew3].radix = 16; // doesn't matter


  /// CV values have param type "Fixed" (for now!)
  // they range from 0 to 32k, the full range of bees values, integers only

  strcpy(desc[eParam_cv0].label, "cv0");
  desc[eParam_cv0].type = eParamTypeFix; 
  desc[eParam_cv0].min = 0x00000000; 
  desc[eParam_cv0].max = PARAM_CV_MAX ;
  desc[eParam_cv0].radix = 16; // full-scale 16.16

  strcpy(desc[eParam_cv1].label, "cv1");
  desc[eParam_cv1].type = eParamTypeFix; 
  desc[eParam_cv1].min = 0x00000000; 
  desc[eParam_cv1].max = PARAM_CV_MAX ;
  desc[eParam_cv1].radix = 16; // full-scale 16.16

  strcpy(desc[eParam_cv2].label, "cv2");
  desc[eParam_cv2].type = eParamTypeFix; 
  desc[eParam_cv2].min = 0x00000000; 
  desc[eParam_cv2].max = PARAM_CV_MAX ;
  desc[eParam_cv2].radix = 16; // full-scale 16.16

  strcpy(desc[eParam_cv3].label, "cv3");
  desc[eParam_cv3].type = eParamTypeFix; 
  desc[eParam_cv3].min = 0x00000000; 
  desc[eParam_cv3].max = PARAM_CV_MAX ;
  desc[eParam_cv3].radix = 16; // full-scale 16.16


  /// slew values have param type "Integrator"
  // they range from 0 to the full positive value of fract32
  /// and will be displayed as seconds-to-convergence by bees.

  strcpy(desc[eParam_cvSlew0].label, "cvSlew0");
  desc[eParam_cvSlew0].type = eParamTypeIntegrator; 
  desc[eParam_cvSlew0].min = 0x00000000; 
  desc[eParam_cvSlew0].max = PARAM_SLEW_MAX;
  desc[eParam_cvSlew0].radix = 16; // doesn't matter

  strcpy(desc[eParam_cvSlew1].label, "cvSlew1");
  desc[eParam_cvSlew1].type = eParamTypeIntegrator; 
  desc[eParam_cvSlew1].min = 0x00000000; 
  desc[eParam_cvSlew1].max = PARAM_SLEW_MAX;
  desc[eParam_cvSlew1].radix = 16; // doesn't matter

  strcpy(desc[eParam_cvSlew2].label, "cvSlew2");
  desc[eParam_cvSlew2].type = eParamTypeIntegrator; 
  desc[eParam_cvSlew2].min = 0x00000000; 
  desc[eParam_cvSlew2].max = PARAM_SLEW_MAX; 
  desc[eParam_cvSlew2].radix = 16; // doesn't matter

  strcpy(desc[eParam_cvSlew3].label, "cvSlew3");
  desc[eParam_cvSlew3].type = eParamTypeIntegrator; 
  desc[eParam_cvSlew3].min = 0x00000000; 
  desc[eParam_cvSlew3].max = PARAM_SLEW_MAX;
  desc[eParam_cvSlew3].radix = 16; // doesn't matter
}

// EOF
