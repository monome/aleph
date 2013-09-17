#ifndef _ALEPH_DRUMSYN_PARAMS_H_
#define _ALEPH_DRUMSYN_PARAMS_H_


#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x1fffffff

enum Params {
  eParamGate0,		
  eParamTrig0,		

  eParamAmp0,		
  eParamAmpSus0,      	
  eParamAmpAtkSlew0,	
  eParamAmpDecSlew0,	
  eParamAmpRelSlew0,	
  eParamAmpSusDur0,	

  eParamFreqOff0,	
  eParamFreqOn0,	
  eParamFreqSus0,	
  eParamFreqAtkSlew0,	
  eParamFreqDecSlew0,	
  eParamFreqRelSlew0,	
  eParamFreqSusDur0,	

  eParamRqOff0,		
  eParamRqOn0,		
  eParamRqSus0,		
  eParamRqAtkSlew0,	
  eParamRqDecSlew0,	
  eParamRqRelSlew0,	
  eParamRqSusDur0,	

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
  eParamFreqOff1,
  eParamFreqOn1,
  eParamRqAtkSlew1,
  eParamRqRelSlew1,
  eParamRqAtkCurve1,
  eParamRqRelCurve1,
  eParamRqOff1,
  eParamRqOn1,
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
  eParamFreqOff2,
  eParamFreqOn2,
  eParamRqAtkSlew2,
  eParamRqRelSlew2,
  eParamRqAtkCurve2,
  eParamRqRelCurve2,
  eParamRqOff2,
  eParamRqOn2,
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
  eParamFreqOff3,
  eParamFreqOn3,
  eParamRqAtkSlew3,
  eParamRqRelSlew 3,
  eParamRqAtkCurve3,
  eParamRqRelCurve3,
  eParamRqOff3,
  eParamRqOn3,
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
