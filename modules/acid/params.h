#ifndef _ALEPH_DRUMSYN_PARAMS_H_
#define _ALEPH_DRUMSYN_PARAMS_H_

// ranges
// svf cutoff
#define PARAM_CUT_MIN     0
#define PARAM_CUT_MAX     0x7fffffff
#define PARAM_CUT_DEFAULT 0x43D0A8EC // ~8k
#define PARAM_CUT_RADIX 1 // doesn't matter

// slew
//// ugh wait... is this right?? these are representation values, not raw coefficients. 
//// apparently they're not being used, or all slews would be tiny.
#define PARAM_SLEW_MIN 0x2000 // 1/8s
#define PARAM_SLEW_MAX 0x400000 // 64s
#define PARAM_SLEW_RADIX 7

// default values
// ~0.001 seconds
#define PARAM_SLEW_1MS 		0x745502CD
// ~0.1 seconds
#define PARAM_SLEW_100MS 	0x7FE09E54
// ~1 second
#define PARAM_SLEW_1S		0x7FFCDCEC

// sustain duration
#define PARAM_DUR_MIN 0x0 // 1/8s
#define PARAM_DUR_MAX 480000 // 10s (why not)
#define PARAM_DUR_RADIX 16 // doesn't actually matter

// rq
#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x0000ffff
#define PARAM_RQ_DEFAULT 0x0000FFF0
#define PARAM_RQ_RADIX 2

// amp
#define PARAM_AMP_0 (0x7fffffff)
#define PARAM_AMP_6 (PARAM_AMP_0 >> 1)
#define PARAM_AMP_12 (PARAM_AMP_0 >> 2)


// number of parameters per voice
// can use as offset hack from param 0
#define PARAM_VOICE_NPARAMS 29
#define PARAM_VOICE_NPARAMS_x2 58
#define PARAM_VOICE_NPARAMS_x3 87

enum Params {
  //  eParamVoice,
  eParamTrig0,
  eParamPreGain0,
  eParamPostGain0,

  eParamAmpAtkSlew0,
  eParamAmpDecSlew0,
  eParamAmpSus0,
  eParamAmpRelSlew0,
  eParamAmpSusDur0,

  eParamFreqSus0,
  eParamFreqAtkSlew0,
  eParamFreqDecSlew0,
  eParamFreqRelSlew0,
  eParamFreqSusDur0,
  eParamFreqOff0,
  eParamFreqOn0,

  eParamRq0,
  eParamLow0,
  eParamHigh0,
  eParamBand0,
  eParamNotch0,
  eParamSvfPre0,
  eParamFreqEnv0,

  eParamNumParams	//
};


// set default values
extern void params_default(void);

#endif // header guard 
// EOF
