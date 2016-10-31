#ifndef _ALEPH_DACS_PARAMS_H_
#define _ALEPH_DACS_PARAMS_H_

#include "param_common.h"

#define PARAM_DAC_MIN 0
#define PARAM_DAC_MAX FR32_MAX
#define PARAM_DAC_RADIX 16
#define OSC_HZ_MIN 0x00010000      // 1 hz
#define OSC_HZ_MAX 0x40000000    // 16384 hz
#define OSC_HZ_RADIX 15

#define RATIO_MIN 0x4000     // 1/4
#define RATIO_MAX 0x40000    // 4
#define RATIO_RADIX 3

#define FADER_TYPE eParamTypeAmp
#define FADER_MIN 0x00000000
#define FADER_MAX FR32_MAX
#define FADER_RADIX 1

#define PAN_TYPE eParamTypeFix
#define PAN_MIN 0x00000000
#define PAN_MAX FR32_MAX
#define PAN_RADIX 16

#define PARAM_AMP_0 (FRACT32_MAX)
#define PARAM_AMP_6 (FRACT32_MAX >> 1)
#define PARAM_AMP_12 (FRACT32_MAX >> 2)

#define PARAM_CV_SLEW_DEFAULT 0x77000000

// cv output
#define PARAM_CV_VAL_DEFAULT PARAM_AMP_6

// enumerate parameters
enum params {
  eParam_cvVal0,
  eParam_cvVal1,
  eParam_cvVal2,
  eParam_cvVal3,
  eParam_cvSlew0,
  eParam_cvSlew1,
  eParam_cvSlew2,
  eParam_cvSlew3,

  eParam_op1Vol,
  eParam_op1Pan,

  eParam_op2Vol,
  eParam_op2Pan,

  eParam_op3Vol,
  eParam_op3Pan,

  eParam_op4Vol,
  eParam_op4Pan,

  /* eParam_Semitone, */
  eParam_noteHz,
  eParam_noteTune,
  eParam_noteTrigger,
  eParam_noteVelocity,

  eParam_op1Mod1Source,
  eParam_op1Mod1Gain,
  eParam_op1Mod2Source,
  eParam_op1Mod2Gain,
  eParam_op1Tune,
  eParam_op1Attack,
  eParam_op1Decay,
  eParam_op1Sustain,
  eParam_op1Release,
  eParam_op1Waveshape,

  eParam_op2Mod1Source,
  eParam_op2Mod1Gain,
  eParam_op2Mod2Source,
  eParam_op2Mod2Gain,
  eParam_op2Tune,
  eParam_op2Attack,
  eParam_op2Decay,
  eParam_op2Sustain,
  eParam_op2Release,
  eParam_op2Waveshape,

  eParam_op3Mod1Source,
  eParam_op3Mod1Gain,
  eParam_op3Mod2Source,
  eParam_op3Mod2Gain,
  eParam_op3Tune,
  eParam_op3Attack,
  eParam_op3Decay,
  eParam_op3Sustain,
  eParam_op3Release,
  eParam_op3Waveshape,

  eParam_op4Mod1Source,
  eParam_op4Mod1Gain,
  eParam_op4Mod2Source,
  eParam_op4Mod2Gain,
  eParam_op4Tune,
  eParam_op4Attack,
  eParam_op4Decay,
  eParam_op4Sustain,
  eParam_op4Release,
  eParam_op4Waveshape,

  eParam_lfoSpeed,
  eParam_lfoWaveshape,

  eParamNumParams

};

extern void fill_param_desc(ParamDesc* desc);

#endif // header guard
// EOF
