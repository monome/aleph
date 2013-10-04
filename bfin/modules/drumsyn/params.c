#include <string.h>
#include "drumsyn.h"
//#include "env_int.h"
#include "env_exp.h"
#include "filter_1p.h"
#include "module.h"
#include "params.h" 

// blechh
static const char* paramStrings[] = {
  "Gate",		// 0
  "Trig",		// 1
  "Amp",		// 2
  "AmpSus",      	// 3
  "AmpAtkSlew",	// 4
  "AmpDecSlew",	// 5
  "AmpRelSlew",	// 6
  "AmpSusDur",	// 7
  "FreqOff",	// 8
  "FreqOn",	// 9
  "FreqSus",	// 10 
  "FreqAtkSlew",	// 11
  "FreqDecSlew",	// 12
  "FreqRelSlew",	// 13
  "FreqSusDur",	// 14
  "RqOff",		// 15
  "RqOn",		// 16
  "RqSus",		// 17
  "RqAtkSlew",	// 18
  "RqDecSlew",	// 19
  "RqRelSlew",	// 20
  "RqSusDur",	// 21
  "Low",		// 22
  "High",		// 23
  "Band",		// 24
  "Notch",		// 25
  "SvfPre",	// 26
  "FreqEnv",	// 27
  "RqEnv",		// 28
};


static void set_param_gate(drumsynVoice* vp, s32 val) {
  if(val > 0) { 
    lcprng_reset(&(vp->rngH), 0xDEADFACE);
    lcprng_reset(&(vp->rngL), 0xDADABEEF);
    env_exp_set_gate( &(vp->envAmp)	, 0xff );
    env_exp_set_gate( &(vp->envFreq)	, 0xff );
    env_exp_set_gate( &(vp->envRq)	, 0xff );
  } else {
    env_exp_set_gate( &(vp->envAmp)	, 0 );
    env_exp_set_gate( &(vp->envFreq)	, 0 );
    env_exp_set_gate( &(vp->envRq)	, 0 );
  }

}

static void set_param_trig(drumsynVoice* vp, s32 val) {
  u8 b = (val > 0);
  env_exp_set_trig( &(vp->envAmp)	, b );
  env_exp_set_trig( &(vp->envFreq)	, b );
  env_exp_set_trig( &(vp->envRq)	, b );
}

static void set_param_svfpre(drumsynVoice* vp, s32 val) {
  // fixme: flip a function pointer or something?
  if(val > 0) {
    vp->svfPre = 1;
  } else {
    vp->svfPre = 0;
  }
}

static void set_param_freqenv(drumsynVoice* vp, s32 val) {
  // fixme: flip a function pointer or something?
  if(val > 0) {
    vp->freqEnv = 1;
  } else {
    vp->freqEnv = 0;
    filter_svf_set_coeff(&(vp->svf), vp->envFreq.valOn);
  }
}

static void set_param_rqenv(drumsynVoice* vp, s32 val) {
  // fixme: flip a function pointer or something?
  if(val > 0) {
    vp->rqEnv = 1;
  } else {
    vp->rqEnv = 0;
    filter_svf_set_rq(&(vp->svf), vp->envRq.valOn);
  }
}

static void module_set_voice_param(u8 vid, u32 idx, pval v);

// set parameter by value
void module_set_param(u32 idx, pval v) {
  /// offset hack on parameter index
  if(idx < PARAM_VOICE_NPARAMS) {
    module_set_voice_param(0, idx, v);
  } else if (idx < PARAM_VOICE_NPARAMS_x2) {
    module_set_voice_param(1, idx - PARAM_VOICE_NPARAMS, v);
  } else if (idx < PARAM_VOICE_NPARAMS_x3) {
    module_set_voice_param(2, idx - PARAM_VOICE_NPARAMS_x2, v);
  } else {
    module_set_voice_param(3, idx - PARAM_VOICE_NPARAMS_x3, v);
  } 
}

//
static void module_set_voice_param(u8 vid, u32 idx, pval v) {
  switch(idx) {

  case eParamGate0 : // 1bit gate
    set_param_gate(voices[vid], v.s);
    break;

  case eParamTrig0 : // 1bit trig
    set_param_trig(voices[vid], v.s);
    break;
    
  case eParamAmp0 : // fract32 amp
    env_exp_set_on( &(voices[vid]->envAmp), v.fr);    
    break;
    
  case eParamAmpSus0 : // fract32 amp
    env_exp_set_sus( &(voices[vid]->envAmp), v.fr);    
    break;
    
  case eParamAmpAtkSlew0 : // fract32 raw 1pole coefficient
    env_exp_set_atk_slew( &(voices[vid]->envAmp), v.fr);
    break;
    
  case eParamAmpDecSlew0 : // fract32 raw 1pole coefficient
    env_exp_set_dec_slew( &(voices[vid]->envAmp), v.fr);
    break;

  case eParamAmpRelSlew0 :
    env_exp_set_rel_slew( &(voices[vid]->envAmp), v.fr);
    break;

  case eParamAmpSusDur0 :
    env_exp_set_sus_dur( &(voices[vid]->envAmp), v.u);
    break;

    // freq env
  case eParamFreqAtkSlew0 :
    env_exp_set_atk_slew( &(voices[vid]->envFreq), v.fr);
    break;

  case eParamFreqDecSlew0 :
    env_exp_set_dec_slew( &(voices[vid]->envFreq), v.fr);
    break;

  case eParamFreqRelSlew0 :
    env_exp_set_rel_slew( &(voices[vid]->envFreq), v.fr);
    break;

  case eParamFreqSusDur0 :
    env_exp_set_sus_dur( &(voices[vid]->envFreq), v.u);
    break;

  case eParamFreqOff0 :
    env_exp_set_off( &(voices[vid]->envFreq), v.fr);
    break;
  case eParamFreqOn0: 
    env_exp_set_on( &(voices[vid]->envFreq), v.fr);
    if(voices[vid]->freqEnv == 0) {
      filter_svf_set_coeff(&(voices[vid]->svf), v.fr);
    }
    break;
  case eParamFreqSus0: 
    env_exp_set_sus( &(voices[vid]->envFreq), v.fr);
    break;

    // rq env
  case eParamRqAtkSlew0 :
    env_exp_set_atk_slew( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqDecSlew0 :
    env_exp_set_dec_slew( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqRelSlew0 :
    env_exp_set_rel_slew( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqSusDur0 :
    env_exp_set_sus_dur( &(voices[vid]->envRq), v.u);
    break;

  case eParamRqOff0 :
    env_exp_set_off( &(voices[vid]->envRq), v.fr);
    break;
  case eParamRqOn0 :
    env_exp_set_on( &(voices[vid]->envRq), v.fr);
    if(voices[vid]->rqEnv == 0) {
      filter_svf_set_rq(&(voices[vid]->svf), v.fr);
    }
    break;
  case eParamRqSus0 :
    env_exp_set_sus( &(voices[vid]->envRq), v.fr);
    break;

  case eParamLow0 :	       
    filter_svf_set_low( &(voices[vid]->svf), v.fr);
    break;
  case eParamHigh0 :	       
    filter_svf_set_high( &(voices[vid]->svf), v.fr);
    break;
  case eParamBand0 :	       
    filter_svf_set_band( &(voices[vid]->svf), v.fr);
    break;
  case eParamNotch0 :	       
    filter_svf_set_notch( &(voices[vid]->svf), v.fr);
    break;

  case eParamSvfPre0 :
    set_param_svfpre(voices[vid], v.s);
    break;	
  case eParamFreqEnv0 :
    set_param_freqenv(voices[vid], v.s);
    break;	
  case eParamRqEnv0 :
    set_param_rqenv(voices[vid], v.s);
    break;		

  default:
    break;
  }
}

void fill_param_desc(void) {
  u32 i, j, k;
  char buf[32] = "";
  const char * numstrs[] = { "0", "1", "2", "3" };
  //char strbuf[32];
  k = 0;
  //  for(i=0; i<eParamNumParams; i++) {
  for(i=0; i<DRUMSYN_NVOICES; i++) {
    for(j=0; j < PARAM_VOICE_NPARAMS; j++) {
    /// ... print to string
      strcpy(buf, paramStrings[j]);
      strcat(buf, numstrs[i]);
      strcpy(gModuleData->paramDesc[k].label, buf);
      strcpy(gModuleData->paramDesc[k].unit, "");
      gModuleData->paramDesc[k].type = PARAM_TYPE_FRACT;
      gModuleData->paramDesc[k].min = 0;
      gModuleData->paramDesc[k].max = FR32_MAX;
      k++;
    }
  }
}
