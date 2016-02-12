#ifndef _ALEPH_RAWSC_PARAMS_H_
#define _ALEPH_RAWSC_PARAMS_H_



#include "param_common.h"

//! produce machine-readable param descriptors
extern void fill_param_desc(ParamDesc* desc);

//! optional: produce readable parameter text
#define PARAM_TEXT
extern void fill_param_text(char** txt);

enum params {

  eParamMix00,
  eParamMix01,
  eParamMix02,
  eParamMix03,

  eParamMix10,
  eParamMix11,
  eParamMix12,
  eParamMix13,

  eParamMix20,
  eParamMix21,
  eParamMix22,
  eParamMix23,

  eParamMix30,
  eParamMix31,
  eParamMix32,
  eParamMix33,

  eParamFreqFine0,
  eParamFreqFine1,
  eParamFreqFine2,
  eParamFreqFine3,
  
  eParamPhase0,
  eParamPhase1,
  eParamPhase2,
  eParamPhase3,
  
  eParamShape0,
  eParamShape1,
  eParamShape2,
  eParamShape3,
  
  eParamFreqCoarse0,
  eParamFreqCoarse1,
  eParamFreqCoarse2,
  eParamFreqCoarse3,
  
  eParamAmp0,
  eParamAmp1,
  eParamAmp2,
  eParamAmp3,

  eParamNumParams
};




#endif
