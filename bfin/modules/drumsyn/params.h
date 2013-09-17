#ifndef _ALEPH_DRUMSYN_PARAMS_H_
#define _ALEPH_DRUMSYN_PARAMS_H_


#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x1fffffff

enum Params {
  eParamGate0,		// 0
  eParamTrig0,		// 1
  eParamAmp0,		// 2
  eParamAmpAtkSlew0,	// 3
  eParamAmpRelSlew0,	// 4
  eParamFreqAtkSlew0,	// 5
  eParamFreqRelSlew0,	// 6
  eParamFreqStart0,	// 7
  eParamFreqEnd0,	// 8
  eParamRqAtkSlew0,	// 9
  eParamRqRelSlew0,	// 10
  eParamRqStart0,	// 11
  eParamRqEnd0,		// 12
  eParamLow0,		// 13
  eParamHigh0,		// 14
  eParamBand0,		// 15
  eParamNotch0,		// 16

  // FIXME
  eParamNumParams

  /*
  eParamGate1,
  eParamTrig1,
  eParamAmp1,
  eParamAmpAtkSlew1,
  eParamAmpRelSlew1,
  eParamAmpAtkCurve1,
  eParamAmpRelCurve1,
  eParamFreqAtkSlew1,
  eParamFreqRelSlew1,
  eParamFreqAtkCurve1,
  eParamFreqRelCurve1,
  eParamFreqStart1,
  eParamFreqEnd1,
  eParamRqAtkSlew1,
  eParamRqRelSlew1,
  eParamRqAtkCurve1,
  eParamRqRelCurve1,
  eParamRqStart1,
  eParamRqEnd1,
  eParamLow1,
  eParamHigh1,
  eParamBand1,
  eParamNotch1,

  eParamGate2,
  eParamTrig2,
  eParamAmp2,
  eParamAmpAtkSlew2,
  eParamAmpRelSlew2,
  eParamAmpAtkCurve2,
  eParamAmpRelCurve2,
  eParamFreqAtkSlew2,
  eParamFreqRelSlew2,
  eParamFreqAtkCurve2,
  eParamFreqRelCurve2,
  eParamFreqStart2,
  eParamFreqEnd2,
  eParamRqAtkSlew2,
  eParamRqRelSlew2,
  eParamRqAtkCurve2,
  eParamRqRelCurve2,
  eParamRqStart2,
  eParamRqEnd2,
  eParamLow2,
  eParamHigh2,
  eParamBand2,
  eParamNotch2, 

  eParamGate3,
  eParamTrig3,
  eParamAmp3,
  eParamAmpAtkSlew3,
  eParamAmpRelSlew3,
  eParamAmpAtkCurve3,
  eParamAmpRelCurve3,
  eParamFreqAtkSlew3,
  eParamFreqRelSlew3,
  eParamFreqAtkCurve3,
  eParamFreqRelCurve3,
  eParamFreqStart3,
  eParamFreqEnd3,
  eParamRqAtkSlew3,
  eParamRqRelSlew 3,
  eParamRqAtkCurve3,
  eParamRqRelCurve3,
  eParamRqStart3,
  eParamRqEnd3,
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
