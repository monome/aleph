
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

// set parameter of voice
extern void ctl_voice_param(u8 vid, u32 pid, fract32 val);


// FIXME: just copying from .ldr source for now... 

// number of parameters per voice
// can use as offset hack from param enum 0
#define PARAM_VOICE_NPARAMS 26
#define PARAM_VOICE_NPARAMS_x2 52
#define PARAM_VOICE_NPARAMS_x3 78


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

  /*
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
  */
  eParamNumParams
};



#endif // h guard

