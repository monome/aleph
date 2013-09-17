
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
  eParamVoice,  // 0
  eParamGate,	// 1	
  eParamTrig,	// 2	

  eParamAmp,	// 3	
  eParamAmpSus, // 4     	
  eParamAmpAtkSlew,	// 5
  eParamAmpDecSlew,	// 6
  eParamAmpRelSlew,	// 7
  eParamAmpSusDur,	// 8

  eParamFreqOff,	// 9
  eParamFreqOn,		// 10
  eParamFreqSus,	// 11
  eParamFreqAtkSlew,	// 12
  eParamFreqDecSlew,	// 13
  eParamFreqRelSlew,	// 14
  eParamFreqSusDur,	// 15

  eParamRqOff,		// 16
  eParamRqOn,		// 17
  eParamRqSus,		// 18
  eParamRqAtkSlew,	// 19
  eParamRqDecSlew,	// 20
  eParamRqRelSlew,	// 21
  eParamRqSusDur,	// 22

  eParamLow,		// 23
  eParamHigh,		// 24
  eParamBand,		// 25
  eParamNotch,	// 26

  // FIXME
  eParamNumParams

} eParam;



#endif // h guard

