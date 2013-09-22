#ifndef _ALEPH_DRUMSYN_PARAMS_H_
#define _ALEPH_DRUMSYN_PARAMS_H_

// ranges
#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x1fffffff

// number of parameters per voice
// can use as offset hack from param 0
#define PARAM_VOICE_NPARAMS 26
#define PARAM_VOICE_NPARAMS_x2 52
#define PARAM_VOICE_NPARAMS_x3 78

enum Params {
  //  eParamVoice,
  eParamGate0,		// 0
  eParamTrig0,		// 1
  eParamAmp0,		// 2
  eParamAmpSus0,      	// 3
  eParamAmpAtkSlew0,	// 4
  eParamAmpDecSlew0,	// 5
  eParamAmpRelSlew0,	// 6
  eParamAmpSusDur0,	// 7
  eParamFreqOff0,	// 8
  eParamFreqOn0,	// 9
  eParamFreqSus0,	// 10 
  eParamFreqAtkSlew0,	// 11
  eParamFreqDecSlew0,	// 12
  eParamFreqRelSlew0,	// 13
  eParamFreqSusDur0,	// 14
  eParamRqOff0,		// 15
  eParamRqOn0,		// 16
  eParamRqSus0,		// 17
  eParamRqAtkSlew0,	// 18
  eParamRqDecSlew0,	// 19
  eParamRqRelSlew0,	// 20
  eParamRqSusDur0,	// 21
  eParamLow0,		// 22
  eParamHigh0,		// 23
  eParamBand0,		// 24
  eParamNotch0,		// 25
  eParamSvfPre0,	// 26
  eParamFreqEnv0,	// 27
  eParamRqEnv0,		// 28

  eParamGate1,		
  eParamTrig1,		
  eParamAmp1,		
  eParamAmpSus1,      	
  eParamAmpAtkSlew1,	
  eParamAmpDecSlew1,	
  eParamAmpRelSlew1,	
  eParamAmpSusDur1,	
  eParamFreqOff1,	
  eParamFreqOn1,	
  eParamFreqSus1,	
  eParamFreqAtkSlew1,	
  eParamFreqDecSlew1,	
  eParamFreqRelSlew1,	
  eParamFreqSusDur1,	
  eParamRqOff1,		
  eParamRqOn1,		
  eParamRqSus1,		
  eParamRqAtkSlew1,	
  eParamRqDecSlew1,	
  eParamRqRelSlew1,	
  eParamRqSusDur1,	
  eParamLow1,		
  eParamHigh1,		
  eParamBand1,		
  eParamNotch1,		
  eParamSvfPre1,	
  eParamFreqEnv1,	
  eParamRqEnv1,		

  eParamGate2,		
  eParamTrig2,		
  eParamAmp2,		
  eParamAmpSus2,      	
  eParamAmpAtkSlew2,	
  eParamAmpDecSlew2,	
  eParamAmpRelSlew2,	
  eParamAmpSusDur2,	
  eParamFreqOff2,	
  eParamFreqOn2,	
  eParamFreqSus2,	
  eParamFreqAtkSlew2,	
  eParamFreqDecSlew2,	
  eParamFreqRelSlew2,	
  eParamFreqSusDur2,	
  eParamRqOff2,		
  eParamRqOn2,		
  eParamRqSus2,		
  eParamRqAtkSlew2,	
  eParamRqDecSlew2,	
  eParamRqRelSlew2,	
  eParamRqSusDur2,	
  eParamLow2,		
  eParamHigh2,		
  eParamBand2,		
  eParamNotch2,		
  eParamSvfPre2,	
  eParamFreqEnv2,	
  eParamRqEnv,		


  eParamGate3,		
  eParamTrig3,		
  eParamAmp3,		
  eParamAmpSus3,      	
  eParamAmpAtkSlew3,	
  eParamAmpDecSlew3,	
  eParamAmpRelSlew3,	
  eParamAmpSusDur3,	
  eParamFreqOff3,	
  eParamFreqOn3,	
  eParamFreqSus3,	
  eParamFreqAtkSlew3,	
  eParamFreqDecSlew3,	
  eParamFreqRelSlew3,	
  eParamFreqSusDur3,	
  eParamRqOff3,		
  eParamRqOn3,		
  eParamRqSus3,		
  eParamRqAtkSlew3,	
  eParamRqDecSlew3,	
  eParamRqRelSlew3,	
  eParamRqSusDur3,	
  eParamLow3,		
  eParamHigh3,		
  eParamBand3,		
  eParamNotch3,				
  eParamSvfPre3,	
  eParamFreqEnv3,	
  eParamRqEnv3,

  eParamNumParams
};

extern void fill_param_desc(void);

#endif // header guard 
// EOF
