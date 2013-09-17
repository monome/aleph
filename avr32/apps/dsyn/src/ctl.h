
#ifndef _ALEPH_APP_FLRY_CTL_H_
#define _ALEPH_APP_FLRY_CTL_H_

#define PARAM_DAC_MIN 0x00000000
#define PARAM_DAC_MAX 0xffff

//---------------------------
//---- -external functions

// query the DSP for parameter descriptors. 
// return 0 if something seems wrong, 1 otherwise
u8 ctl_report_params(void);
// set parameters to initial values
void ctl_init_params(void);
// set gate
void  ctl_set_gate(u8 ch, u8 val);

// parameter enum
// FIXME: just copying from .ldr source for now

typedef enum Params {
  eParamVoice,
  eParamGate,		
  eParamTrig,		
  eParamAmp,		
  eParamAmpSus,      	
  eParamAmpAtkSlew,	
  eParamAmpDecSlew,	
  eParamAmpRelSlew,	
  eParamAmpSusDur,	
  eParamFreqOff,	
  eParamFreqOn,	
  eParamFreqSus,	
  eParamFreqAtkSlew,	
  eParamFreqDecSlew,	
  eParamFreqRelSlew,	
  eParamFreqSusDur,	
  eParamRqOff,		
  eParamRqOn,		
  eParamRqSus,		
  eParamRqAtkSlew,	
  eParamRqDecSlew,	
  eParamRqRelSlew,	
  eParamRqSusDur,	
  eParamLow,		
  eParamHigh,		
  eParamBand,		
  eParamNotch,	

  // FIXME
  eParamNumParams

} eParam;



#endif // h guard

