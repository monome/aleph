
#ifndef _ALEPH_APP_FLRY_CTL_H_
#define _ALEPH_APP_FLRY_CTL_H_

#include "types.h"

// parameter min/max
#define PARAM_DAC_MIN 0x00000000
#define PARAM_DAC_MAX 0xffff

// count of available voices
#define DSYN_NVOICES 4

//---------------------------
//---- -external functions

// query the DSP for parameter descriptors. 
// return 0 if something seems wrong, 1 otherwise
extern u8 ctl_report_params(void);
// set parameters to initial values
extern void ctl_init_params(void);
// set gate
extern void  ctl_set_gate(u8 ch, u8 val);
// increment tempo
extern void ctl_inc_tempo(s32 val);
// increment selected DSP parameter
extern void ctl_inc_param(u8 vid, s32 val);
// increment parameter selection
extern void ctl_inc_param_select(u8 vid, s32 inc);

// set parameter of voice
extern void ctl_voice_param(u8 vid, u32 pid, fract32 val);


// FIXME: just copying from .ldr source for now... 

// number of parameters per voice
// can use as offset hack from param enum 0
#define PARAM_VOICE_NPARAMS 29
#define PARAM_VOICE_NPARAMS_x2 58
#define PARAM_VOICE_NPARAMS_x3 87


// parameter enum
enum Params {
  //  eParamVoice,
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
  eParamSvfPre,	
  eParamFreqEnv,	
  eParamRqEnv,		
  eParamNumParams
};



#endif // h guard

