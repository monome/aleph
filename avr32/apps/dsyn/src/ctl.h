
#ifndef _ALEPH_APP_FLRY_CTL_H_
#define _ALEPH_APP_FLRY_CTL_H_

#include "types.h"

//-------------------------------------------
//-- parameters

// FIXME: just copying from .ldr source for now... 
// could match reports from bfin after load, so DSP enumeration is free to change

// count of available voices
#define DSYN_NVOICES 4

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
  eParamNumVoiceParams
};

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

// get input value
const u32 ctl_get_inval (u8 vid, u32 pid);

#endif // h guard

