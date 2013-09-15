#ifndef _ALEPH_DRUMSYN_PARAMS_H_
#define _ALEPH_DRUMSYN_PARAMS_H_


#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x1fffffff

enum Params {
  eParamGate0,		// 0
  eParamTrig0,		// 1
  eParamAmp0,		// 2
  eParamAmpAtkDur0,	// 3
  eParamAmpRelDur0,	// 4
  eParamAmpAtkCurve0,	// 5
  eParamAmpRelCurve0,	// 6
  eParamFreqAtkDur0,	// 7
  eParamFreqRelDur0,	// 8
  eParamFreqAtkCurve0,	// 9
  eParamFreqRelCurve0,	// 10
  eParamFreqMul0,	// 11
  eParamFreqAdd0,	// 12
  eParamResAtkDur0,	// 13
  eParamResRelDur0,	// 14
  eParamResAtkCurve0,	// 15
  eParamResRelCurve0,	// 16 
  eParamResMul0,	// 17
  eParamResAdd0,	// 18
  eParamLow0,		// 19
  eParamHigh0,		// 20
  eParamBand0,		// 21
  eParamNotch0,		// 22

  // FIXME
  eParamNumParams

  /*
  eParamGate1,
  eParamTrig1,
  eParamAmp1,
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
  eParamLow1,
  eParamHigh1,
  eParamBand1,
  eParamNotch1,

  eParamGate2,
  eParamTrig2,
  eParamAmp2,
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
  eParamLow2,
  eParamHigh2,
  eParamBand2,
  eParamNotch2, 

  eParamGate3,
  eParamTrig3,
  eParamAmp3,
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
  eParamLow3,
  eParamHigh3,
  eParamBand3,
  eParamNotch3,

    eParamNumParams
  */
};

extern void fill_param_desc(void);

#endif // header guard 
// EOF
