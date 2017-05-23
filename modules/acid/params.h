#ifndef _ALEPH_DRUMSYN_PARAMS_H_
#define _ALEPH_DRUMSYN_PARAMS_H_

// ranges
// svf cutoff
#define PARAM_CUT_MIN     0
#define PARAM_CUT_MAX     0x7fffffff
#define PARAM_CUT_DEFAULT 0x43D0A8EC >> 5 // ~8k
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
#define PARAM_RQ_DEFAULT 0x0000ffff >> 5
#define PARAM_RQ_RADIX 2

// amp
#define PARAM_AMP_0 (0x7fffffff)
#define PARAM_AMP_6 (PARAM_AMP_0 >> 1)
#define PARAM_AMP_12 (PARAM_AMP_0 >> 2)


enum dsynParams {
  //  dsynParamVoice,
  dsynParamTrig, // 0
  dsynParamPreGain, // 1
  dsynParamPostGain, // 2

  dsynParamAmpAtkSlew, // 3
  dsynParamAmpDecSlew, // 4
  dsynParamAmpSus, // 5
  dsynParamAmpRelSlew, // 6
  dsynParamAmpSusDur, // 7

  dsynParamFreqAtkSlew, // 8
  dsynParamFreqDecSlew, // 9
  dsynParamFreqSus, // 10
  dsynParamFreqRelSlew, // 11
  dsynParamFreqSusDur, // 12
  dsynParamFreqOff, // 13
  dsynParamFreqOn, // 14

  dsynParamRq, // 15
  dsynParamLow, // 16
  dsynParamHigh, // 17
  dsynParamBand, // 18
  dsynParamNotch, // 19
  dsynParamSvfPre, // 20
  dsynParamFreqEnv, // 21

  dsynNumParams	//
};

enum monosynthParams {
  // monosynth voice params
  monosynthParamNoteTrigger,
  monosynthParamNoteHz,
  monosynthParamNoteTune,
  monosynthParamOscFlava,

  monosynthParamAmpAttack,
  monosynthParamAmpDecay,
  monosynthParamAmpSustain,
  monosynthParamAmpRelease,
  monosynthParamNoteLevel,

  monosynthParamFiltAttack,
  monosynthParamFiltDecay,
  monosynthParamFiltSustain,
  monosynthParamFiltRelease,
  monosynthParamFreqOff,
  monosynthParamFreqOn,

  monosynthParamRq,
  monosynthParamDist,
  
  monosynthNumParams
};

#define DRUMSYN_NVOICES 1
#define MONOSYNTH_NVOICES 1

#define DSYN_NPARAMS (DRUMSYN_NVOICES * dsynNumParams)
#define MONOSYNTH_NPARAMS (MONOSYNTH_NVOICES * monosynthNumParams)

#define ACID_NPARAMS (DSYN_NPARAMS + MONOSYNTH_NPARAMS)

// set default values
extern void params_default(void);

#endif // header guard 
// EOF
