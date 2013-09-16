
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

} eParam;



#endif // h guard

