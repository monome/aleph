#include <string.h>
#include "module.h"
#include "param_common.h"
#include "params.h" 

static ParamDesc voiceParamDescData[] = {

  // trig/gate
  { .label = "Trig", 	.type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },

  // amp env
  { .label = "PreGain", 	.type = eParamTypeAmp, .min = 0, .max = PARAM_AMP_0, .radix = 1 },
  { .label = "PostGain", 	.type = eParamTypeAmp, .min = 0, .max = PARAM_AMP_0, .radix = 1 },
  { .label = "AmpAttack", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "AmpDecay", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "AmpSustain", 	.type = eParamTypeAmp, .min = 0, .max = PARAM_AMP_0, .radix = 1 },
  { .label = "AmpRelease", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "AmpDuration", .type = eParamTypeShort,
    .min = PARAM_DUR_MIN, .max = PARAM_DUR_MAX, .radix = PARAM_DUR_RADIX
  },
  // cutoff env
  { .label = "FreqAttack", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "FreqDecay", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "FreqSustain", .type =eParamTypeFix,
    .min = 0, .max = FR32_MAX, .radix = 16
  },
  { .label = "FreqRelease", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "FreqDuration", .type = eParamTypeShort,
    .min = PARAM_DUR_MIN, .max = PARAM_DUR_MAX, .radix = PARAM_DUR_RADIX
  }, 
  { .label = "FreqOff", .type = eParamTypeSvfFreq,
    .min = PARAM_CUT_MIN, .max = PARAM_CUT_MAX, .radix = PARAM_CUT_RADIX
  },
  { .label = "FreqOn", .type = eParamTypeSvfFreq,
    .min = PARAM_CUT_MIN, .max = PARAM_CUT_MAX, .radix = PARAM_CUT_RADIX
  },

  // rq env
  { .label = "Rq", .type = eParamTypeFix,
    .min = PARAM_RQ_MIN, .max = PARAM_RQ_MAX, .radix = PARAM_RQ_RADIX
  },
  {  .label = "Low", .type = eParamTypeAmp, .min = 0, .max = PARAM_AMP_0, .radix = 1 },
  {  .label = "High", .type = eParamTypeAmp, .min=0, .max = PARAM_AMP_0, .radix = 1 },
  { .label =  "Band", .type = eParamTypeAmp, .min=0, .max = PARAM_AMP_0, .radix = 1 },
  { .label = "Notch", .type = eParamTypeAmp, .min=0, .max = PARAM_AMP_0, .radix = 1 },
  
  // routing
  { .label = "SvfPre", .type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },
  // env enable
  { .label = "FreqEnv", .type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },
};

#define OSC_HZ_MIN 0x00010000      // 1 hz
#define OSC_HZ_MAX 0x40000000    // 16384 hz
#define OSC_HZ_RADIX 15

#define RATIO_MIN 0x4000     // 1/4
#define RATIO_MAX 0x40000    // 4
#define RATIO_RADIX 3

static ParamDesc monosynthParamDescData[] = {

  // trig/gate
  { .label = "Trig", 	.type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },

  { .label = "NoteHz", .type = eParamTypeNote,
    .min = OSC_HZ_MIN, .max = OSC_HZ_MAX, .radix = OSC_HZ_RADIX
  },
  { .label = "NoteTune", .type = eParamTypeFix,
    .min = RATIO_MIN, .max = RATIO_MAX, .radix = RATIO_RADIX
  },
  { .label =  "Vol", .type = eParamTypeAmp, .min=0, .max = PARAM_AMP_0, .radix = 1 },
};


void fill_param_desc(ParamDesc* desc) {
  u32 i, j, k;
  const char * numstrs[] = { "d0", "d1", "d2", "d3" };
  ParamDesc* descData;

  k = 0;
  for(i=0; i<DRUMSYN_NVOICES; i++) {
    for(j=0; j < dsynNumParams; j++) { 
      descData = &(voiceParamDescData[j]);
      memcpy( &(desc[k]), descData, sizeof(ParamDesc) );
      strcat(desc[k].label, numstrs[i]);
      ++k;
    }
  }
  const char * monoSuffix [] = {"m0", "m1"};
  for(i=0; i<MONOSYNTH_NVOICES; i++) {
    for(j=0; j < monosynthNumParams; j++) { 
      descData = &(monosynthParamDescData[j]);
      memcpy( &(desc[k]), descData, sizeof(ParamDesc) );
      strcat(desc[k].label, monoSuffix[i]);
      ++k;
    }
  }
}
