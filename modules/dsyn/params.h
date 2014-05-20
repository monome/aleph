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
  eParamGate0,		// 0
  eParamTrig0,		// 1
  eParamAmp0,		// 2
  eParamAmpSus0,      	// 3
  eParamAmpAtkSlew0,	// 4
  eParamAmpDecSlew0,	// 5
  eParamAmpRelSlew0,	// 6
  eParamAmpSusDur0,	// 7
  eParamFreqOff0,	// 8
  eParamFreqOn0,	// 9
  eParamFreqSus0,	// 10 
  eParamFreqAtkSlew0,	// 11
  eParamFreqDecSlew0,	// 12
  eParamFreqRelSlew0,	// 13
  eParamFreqSusDur0,	// 14
  eParamRqOff0,		// 15
  eParamRqOn0,		// 16
  eParamRqSus0,		// 17
  eParamRqAtkSlew0,	// 18
  eParamRqDecSlew0,	// 19
  eParamRqRelSlew0,	// 20
  eParamRqSusDur0,	// 21
  eParamLow0,		// 22
  eParamHigh0,		// 23
  eParamBand0,		// 24
  eParamNotch0,		// 25
  eParamSvfPre0,	// 26
  eParamFreqEnv0,	// 27
  eParamRqEnv0,		// 28

  eParamGate1,	        // 29
  eParamTrig1,		// 30
  eParamAmp1,		// 31
  eParamAmpSus1,      	// 32
  eParamAmpAtkSlew1,	// 33
  eParamAmpDecSlew1,	// 34
  eParamAmpRelSlew1,	// 35 
  eParamAmpSusDur1,	// 36 
  eParamFreqOff1,	// 37 
  eParamFreqOn1,	// 38
  eParamFreqSus1,	// 39
  eParamFreqAtkSlew1,	// 40
  eParamFreqDecSlew1,	// 41
  eParamFreqRelSlew1,	// 42
  eParamFreqSusDur1,	// 43
  eParamRqOff1,		// 44
  eParamRqOn1,		// 45
  eParamRqSus1,		// 46
  eParamRqAtkSlew1,	// 47
  eParamRqDecSlew1,	// 48
  eParamRqRelSlew1,	// 49
  eParamRqSusDur1,	// 50
  eParamLow1,		// 51
  eParamHigh1,		// 52
  eParamBand1,		// 53
  eParamNotch1,		// 54
  eParamSvfPre1,	// 55
  eParamFreqEnv1,	// 56
  eParamRqEnv1,		// 57
 
  eParamGate2,		// 58
  eParamTrig2,		// 59
  eParamAmp2,		// 60
  eParamAmpSus2,      	// 61
  eParamAmpAtkSlew2,	// 62
  eParamAmpDecSlew2,	// 63
  eParamAmpRelSlew2,	// 64
  eParamAmpSusDur2,	// 65
  eParamFreqOff2,	// 66
  eParamFreqOn2,	// 67
  eParamFreqSus2,	// 68
  eParamFreqAtkSlew2,	// 69
  eParamFreqDecSlew2,	// 70
  eParamFreqRelSlew2,	// 71
  eParamFreqSusDur2,	// 72
  eParamRqOff2,		// 73
  eParamRqOn2,		// 74
  eParamRqSus2,		// 75
  eParamRqAtkSlew2,	// 76
  eParamRqDecSlew2,	// 77
  eParamRqRelSlew2,	// 78
  eParamRqSusDur2,	// 79
  eParamLow2,		// 80 
  eParamHigh2,		// 81
  eParamBand2,		// 82
  eParamNotch2,		// 83
  eParamSvfPre2,	// 84
  eParamFreqEnv2,	// 85
  eParamRqEnv,		// 86

  eParamGate3,		// 87
  eParamTrig3,		// 88
  eParamAmp3,		// 89
  eParamAmpSus3,      	// 90
  eParamAmpAtkSlew3,	// 91
  eParamAmpDecSlew3,	// 92
  eParamAmpRelSlew3,	// 93
  eParamAmpSusDur3,	// 94
  eParamFreqOff3,	// 95
  eParamFreqOn3,	// 96
  eParamFreqSus3,	// 97
  eParamFreqAtkSlew3,	// 98
  eParamFreqDecSlew3,	// 99
  eParamFreqRelSlew3,	// 100 
  eParamFreqSusDur3,	// 101
  eParamRqOff3,		// 102
  eParamRqOn3,		// 103
  eParamRqSus3,		// 104
  eParamRqAtkSlew3,	// 105
  eParamRqDecSlew3,	// 106
  eParamRqRelSlew3,	// 107
  eParamRqSusDur3,	// 108
  eParamLow3,		// 109
  eParamHigh3,		// 110
  eParamBand3,		// 111
  eParamNotch3,		// 112		
  eParamSvfPre3,	// 113
  eParamFreqEnv3,	// 114
  eParamRqEnv3,		// 115

  eParamNumParams	//
};


// set default values
extern void params_default(void);

#endif // header guard 
// EOF
