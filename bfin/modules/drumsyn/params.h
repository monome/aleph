#ifndef _ALEPH_DRUMSYN_PARAMS_H_
#define _ALEPH_DRUMSYN_PARAMS_H_


#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x1fffffff

typedef enum Params {
  eParamVoice,
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

  eParamNumParams
  
} eParam;


extern void fill_param_desc(void);

#endif // header guard 
// EOF
