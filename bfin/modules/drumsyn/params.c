#include <string.h>
#include "drumsyn.h"
//#include "env_int.h"
#include "env_exp.h"
#include "filter_1p.h"
#include "module.h"
#include "params.h" 

// current voice target
static u8 vid = 0;

// set gate for voice
static void set_param_gate(drumsynVoice* vp, s32 val) {
  if(val > 0) { 
    lcprng_reset(&(vp->rngH));
    lcprng_reset(&(vp->rngL));
    env_exp_set_gate( &(vp->envAmp)	, 0xff );
    env_exp_set_gate( &(vp->envFreq)	, 0xff );
    env_exp_set_gate( &(vp->envRq)	, 0xff );
  } else {
    env_exp_set_gate( &(vp->envAmp)	, 0 );
    env_exp_set_gate( &(vp->envFreq)	, 0 );
    env_exp_set_gate( &(vp->envRq)	, 0 );
  }
}

// set parameter by value
void module_set_param(u32 idx, pval v) { 
  switch(idx) {
  case eParamVoice :
    vid = v.u % (DRUMSYN_NVOICES);    // set voice index
    break;
  case eParamGate : // 1bit gate
    set_param_gate(voices[vid], v.s);
    break;    
  case eParamAmp : // fract32 amp
    env_exp_set_on( &(voices[vid]->envAmp), v.fr);    
    break;    
  case eParamAmpSus : // fract32 amp
    env_exp_set_sus( &(voices[vid]->envAmp), v.fr);    
    break;    
  case eParamAmpAtkSlew : // fract32 raw 1pole coefficient
    env_exp_set_atk_slew( &(voices[vid]->envAmp), v.fr);
    break;    
  case eParamAmpDecSlew : // fract32 raw 1pole coefficient
    env_exp_set_dec_slew( &(voices[vid]->envAmp), v.fr);
    break;
  case eParamAmpRelSlew :
    env_exp_set_rel_slew( &(voices[vid]->envAmp), v.fr);
    break;
  case eParamAmpSusDur :
    env_exp_set_sus_dur( &(voices[vid]->envAmp), v.u);
    break;
    // freq env
  case eParamFreqAtkSlew :
    env_exp_set_atk_slew( &(voices[vid]->envFreq), v.fr);
    break;
  case eParamFreqDecSlew :
    env_exp_set_dec_slew( &(voices[vid]->envFreq), v.fr);
    break;
  case eParamFreqRelSlew :
    env_exp_set_rel_slew( &(voices[vid]->envFreq), v.fr);
    break;
  case eParamFreqSusDur :
    env_exp_set_sus_dur( &(voices[vid]->envFreq), v.u);
    break;
  case eParamFreqOff :
    env_exp_set_off( &(voices[vid]->envFreq), v.fr);
    break;
  case eParamFreqOn : 
    env_exp_set_on( &(voices[vid]->envFreq), v.fr);
    break;
  case eParamFreqSus : 
    env_exp_set_sus( &(voices[vid]->envFreq), v.fr);
    break;
    // rq env
  case eParamRqAtkSlew :
    env_exp_set_atk_slew( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqDecSlew :
    env_exp_set_dec_slew( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqRelSlew :
    env_exp_set_rel_slew( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqSusDur :
    env_exp_set_sus_dur( &(voices[vid]->envRq), v.u);
    break;
  case eParamRqOff :
    env_exp_set_off( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqOn :
    env_exp_set_on( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqSus :
    env_exp_set_sus( &(voices[vid]->envRq), v.fr);
    break;
    // svf
  case eParamLow :	       
    filter_svf_set_low( &(voices[vid]->svf), v.fr);
    break;
  case eParamHigh :	       
    filter_svf_set_high( &(voices[vid]->svf), v.fr);
    break;
  case eParamBand :	       
    filter_svf_set_band( &(voices[vid]->svf), v.fr);
    break;
  case eParamNotch : 	       
    filter_svf_set_notch( &(voices[vid]->svf), v.fr);
    break;
    //// TODO
    /// .. other voices...
  default:
    break;
  }
}

// fill parameter descriptors
void fill_param_desc(void) {
  // FIXME: not using most param descriptor fields.
  // all params are unit-less fract32 in range [0, 1).
  // it is pretty painful to use them all.
  // either need scripting tools to handle this stuff ASAP,
  // or something like u32 index into (extensive) shared table of unit types.

  strcpy(gModuleData->paramDesc[eParamGate].label, "Gate");
  strcpy(gModuleData->paramDesc[eParamTrig].label, "Trig");
  // amp
  strcpy(gModuleData->paramDesc[eParamAmp].label, "Amp");
  strcpy(gModuleData->paramDesc[eParamAmpSus].label, "AmpSus");
  strcpy(gModuleData->paramDesc[eParamAmpSusDur].label, "AmpSusDur");
  strcpy(gModuleData->paramDesc[eParamAmpAtkSlew].label, "AmpAtkSlew");
  strcpy(gModuleData->paramDesc[eParamAmpDecSlew].label, "AmpDecSlew");
  strcpy(gModuleData->paramDesc[eParamAmpRelSlew].label, "AmpRelSlew");
  // freq
  strcpy(gModuleData->paramDesc[eParamFreqAtkSlew].label, "FreqAtkSlew");
  strcpy(gModuleData->paramDesc[eParamFreqDecSlew].label, "FreqDecSlew");
  strcpy(gModuleData->paramDesc[eParamFreqRelSlew].label, "FreqRelSlew");
  strcpy(gModuleData->paramDesc[eParamFreqOff].label, "FreqOff");
  strcpy(gModuleData->paramDesc[eParamFreqOn].label, "FreqOn");
  strcpy(gModuleData->paramDesc[eParamFreqSus].label, "FreqSus");
  strcpy(gModuleData->paramDesc[eParamFreqSusDur].label, "FreqSusDur");
  // rq
  strcpy(gModuleData->paramDesc[eParamRqAtkSlew].label, "RqAtkSlew");
  strcpy(gModuleData->paramDesc[eParamRqDecSlew].label, "RqDecSlew");
  strcpy(gModuleData->paramDesc[eParamRqRelSlew].label, "RqRelSlew");
  strcpy(gModuleData->paramDesc[eParamRqOff].label, "RqOff");
  strcpy(gModuleData->paramDesc[eParamRqOn].label, "RqOn");
  strcpy(gModuleData->paramDesc[eParamRqSus].label, "RqSus");
  strcpy(gModuleData->paramDesc[eParamRqSusDur].label, "RqSusDur");
  // svf
  strcpy(gModuleData->paramDesc[eParamLow].label, "Low");
  strcpy(gModuleData->paramDesc[eParamHigh].label, "High");
  strcpy(gModuleData->paramDesc[eParamBand].label, "Band");
  strcpy(gModuleData->paramDesc[eParamNotch].label, "Notch");
  // voice selection
  strcpy(gModuleData->paramDesc[eParamVoice].label, "Low3");
}
