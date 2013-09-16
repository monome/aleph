#ifndef _ALEPH_DRUMSYN_PARAMS_H_
#define _ALEPH_DRUMSYN_PARAMS_H_


#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x1fffffff

enum Params {
  eParamGate0,		
  eParamTrig0,		
  eParamAmp0,		
  eParamAmpAtkSlew0,	
  eParamAmpRelSlew0,	
  //  eParamAmpAtkCurve0	
  //  eParamAmpRelCurve0,	
  eParamFreqAtkSlew0,	
  eParamFreqRelSlew0,	
  //  eParamFreqAtkCurve0,	
  //  eParamFreqRelCurve0,	
  eParamFreqStart0,	
  eParamFreqEnd0,	
  eParamResAtkSlew0,	
  eParamResRelSlew0,	
  eParamResAtkCurve0,	
  eParamResRelCurve0,	
  eParamResStart0,	
  eParamResEnd0,	
  eParamLow0,		
  eParamHigh0,		
  eParamBand0,		
  eParamNotch0,		

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
  eParamResAtkSlew1,
  eParamResRelSlew1,
  eParamResAtkCurve1,
  eParamResRelCurve1,
  eParamResStart1,
  eParamResEnd1,
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
  eParamResAtkSlew2,
  eParamResRelSlew2,
  eParamResAtkCurve2,
  eParamResRelCurve2,
  eParamResStart2,
  eParamResEnd2,
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
  eParamResAtkSlew3,
  eParamResRelSlew3,
  eParamResAtkCurve3,
  eParamResRelCurve3,
  eParamResStart3,
  eParamResEnd3,
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
