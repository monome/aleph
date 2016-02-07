#ifndef _ALEPH_RAWSC_PARAMS_H_
#define _ALEPH_RAWSC_PARAMS_H_



#include "param_common.h"

//! produce machine-readable param descriptors
extern void fill_param_desc(ParamDesc* desc);

//! optional: produce readable parameter text
#define PARAM_TEXT
extern void fill_param_text(char** txt);

enum params {
  eParamFreqFine0,
  eParamFreqFine1,
  eParamFreqFine2,
  eParamFreqFine3,

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
