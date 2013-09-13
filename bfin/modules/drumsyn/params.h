#ifndef _ALEPH_DRUMS_PARAMS_H_
#define _ALEPH_DRUMS_PARAMS_H_

enum Params {
  eParamGate0,
  eParamAmpAtkDur0,
  eParamAmpRelDur0,
  eParamAmpAtkCurve0,
  eParamAmpRelCurve0,
  eParamFreqAtkDur0,
  eParamFreqRelDur0,
  eParamFreqAtkCurve0,
  eParamFreqRelCurve0,
  eParamFreqMul0,
  eParamFreqAdd0,
  eParamResAtkDur0,
  eParamResRelDur0,
  eParamResAtkCurve0,
  eParamResRelCurve0,
  eParamResMul0,
  eParamResAdd0,

  eParamGate1,
  eParamAmpAtkDur1,
  eParamAmpRelDur1,
  eParamAmpAtkCurve1,
  eParamAmpRelCurve1,
  eParamFreqAtkDur1,
  eParamFreqRelDur1,
  eParamFreqAtkCurve1,
  eParamFreqRelCurve1,
  eParamFreqMul1,
  eParamFreqAdd1,
  eParamResAtkDur1,
  eParamResRelDur1,
  eParamResAtkCurve1,
  eParamResRelCurve1,
  eParamResMul1,
  eParamResAdd1,

  eParamGate2,
  eParamAmpAtkDur2,
  eParamAmpRelDur2,
  eParamAmpAtkCurve2,
  eParamAmpRelCurve2,
  eParamFreqAtkDur2,
  eParamFreqRelDur2,
  eParamFreqAtkCurve2,
  eParamFreqRelCurve2,
  eParamFreqMul2,
  eParamFreqAdd2,
  eParamResAtkDur2,
  eParamResRelDur2,
  eParamResAtkCurve2,
  eParamResRelCurve2,
  eParamResMul2,
  eParamResAdd2, 

  eParamGate3,
  eParamAmpAtkDur3,
  eParamAmpRelDur3,
  eParamAmpAtkCurve3,
  eParamAmpRelCurve3,
  eParamFreqAtkDur3,
  eParamFreqRelDur3,
  eParamFreqAtkCurve3,
  eParamFreqRelCurve3,
  eParamFreqMul3,
  eParamFreqAdd3,
  eParamResAtkDur3,
  eParamResRelDur3,
  eParamResAtkCurve3,
  eParamResRelCurve3,
  eParamResMul3,
  eParamResAdd3,
  eParamNumParams
};

/* enum params { */
/*   eParamGate, */
/*   eParamAtkDur, */
/*   eParamRelDur, */
/*   eParamAtkCurve, */
/*   eParamRelCurve, */
/*   eParamSvfHz, */
/*   eParamSvfRq, */
/*   eParamSvfLow, */
/*   eParamSvfHigh, */
/*   eParamSvfBand, */
/*   eParamSvfNotch, */
/*   eParamSvfPeak, */
/*   eParamNoiseAmp, */
/*   eParamInAmp0, */
/*   eParamInAmp1, */
/*   eParamInAmp2, */
/*   eParamInAmp3, */
/*   eParamNumParams */
/* }; */

extern void fill_param_desc(void);

#endif // header guard 
// EOF
