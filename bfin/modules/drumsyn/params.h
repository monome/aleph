#ifndef _ALEPH_DRUMS_PARAMS_H_
#define _ALEPH_DRUMS_PARAMS_H_


enum params {
  eParamGate,
  eParamAtkDur,
  eParamRelDur,
  eParamAtkCurve,
  eParamRelCurve,
  eParamSvfHz,
  eParamSvfRq,
  eParamSvfLow,
  eParamSvfHigh,
  eParamSvfBand,
  eParamSvfNotch,
  eParamSvfPeak,
  eParamNoiseAmp,
  eParamInAmp0,
  eParamInAmp1,
  eParamInAmp2,
  eParamInAmp3,
  eParamNumParams
};

// populate parameter descriptor fields.
void fill_param_desc(void);

#endif // header guard 
// EOF
