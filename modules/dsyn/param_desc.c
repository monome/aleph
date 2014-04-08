#include <string.h>
#include "module.h"
#include "dsyn.h"
#include "param_common.h"
#include "params.h" 

static ParamDesc voiceParamDescData[] = {

  // trig/gate
  { .label = "Gate", 	.type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },
  { .label = "Trig", 	.type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },

  // amp env
  { .label = "Amp", 	.type = eParamTypeAmp, .min = 0, .max = PARAM_AMP_0, .radix = 1 },
  { .label = "AmpSus", 	.type = eParamTypeAmp, .min = 0, .max = PARAM_AMP_0, .radix = 1 },
  { .label = "AmpAtkSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "AmpDecSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "AmpRelSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "AmpSusDur", .type = eParamTypeShort,
    .min = PARAM_DUR_MIN, .max = PARAM_DUR_MAX, .radix = PARAM_DUR_RADIX
  },
  // cutoff env
  { .label = "FreqOff", .type = eParamTypeSvfFreq,
    .min = PARAM_CUT_MIN, .max = PARAM_CUT_MAX, .radix = PARAM_CUT_RADIX
  },
  { .label = "FreqOn", .type = eParamTypeSvfFreq,
    .min = PARAM_CUT_MIN, .max = PARAM_CUT_MAX, .radix = PARAM_CUT_RADIX
  },
  { .label = "FreqSus", .type = eParamTypeSvfFreq,
    .min = PARAM_CUT_MIN, .max = PARAM_CUT_MAX, .radix = PARAM_CUT_RADIX
  },
  { .label = "FreqAtkSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "FreqDecSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "FreqRelSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "FreqSusDur", .type = eParamTypeShort,
    .min = PARAM_DUR_MIN, .max = PARAM_DUR_MAX, .radix = PARAM_DUR_RADIX
  }, 

  // rq env
  { .label = "RqOff", .type = eParamTypeFix,
    .min = PARAM_RQ_MIN, .max = PARAM_RQ_MAX, .radix = PARAM_RQ_RADIX
  },
  { .label = "RqOn", .type = eParamTypeFix,
    .min = PARAM_RQ_MIN, .max = PARAM_RQ_MAX, .radix = PARAM_RQ_RADIX
  },
  { .label = "RqSus", .type = eParamTypeFix,
    .min = PARAM_RQ_MIN, .max = PARAM_RQ_MAX, .radix = PARAM_RQ_RADIX
  },
  { .label = "RqAtkSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "RqDecSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "RqRelSlew", .type = eParamTypeIntegrator,
    .min = PARAM_SLEW_MIN, .max = PARAM_SLEW_MAX, .radix = PARAM_SLEW_RADIX
  },
  { .label = "RqSusDur", .type = eParamTypeShort,
    .min = PARAM_DUR_MIN, .max = PARAM_DUR_MAX, .radix = PARAM_DUR_RADIX
  },

  {  .label = "Low", .type = eParamTypeAmp, .min = 0, .max = PARAM_AMP_0, .radix = 1 },
  {  .label = "High", .type = eParamTypeAmp, .min=0, .max = PARAM_AMP_0, .radix = 1 },
  { .label =  "Band", .type = eParamTypeAmp, .min=0, .max = PARAM_AMP_0, .radix = 1 },
  { .label = "Notch", .type = eParamTypeAmp, .min=0, .max = PARAM_AMP_0, .radix = 1 },
  
  // routing
  { .label = "SvfPre", .type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },
  // env enable
  { .label = "FreqEnv", .type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },
  { .label = "RqEnv", .type = eParamTypeBool, .min = 0, .max = 1, .radix = 2 },
};

void fill_param_desc(ParamDesc* desc) {
  u32 i, j, k;
  char buf[32] = "";
  const char * numstrs[] = { "0", "1", "2", "3" };
  ParamDesc* descData;

  k = 0;
  for(i=0; i<DRUMSYN_NVOICES; i++) { 
    for(j=0; j < PARAM_VOICE_NPARAMS; j++) { 
      descData = &(voiceParamDescData[j]);
      memcpy( &(desc[k]), descData, sizeof(ParamDesc) );
      strcat(desc[k].label, numstrs[i]);
      ++k;
    }
  }
}

/* static const char* paramStrings[] = { */
/*   "Gate",		// 0 */
/*   "Trig",		// 1 */
/*   "Amp",		// 2 */
/*   "AmpSus",      	// 3 */
/*   "AmpAtkSlew",	// 4 */
/*   "AmpDecSlew",	// 5 */
/*   "AmpRelSlew",	// 6 */
/*   "AmpSusDur",	// 7 */
/*   "FreqOff",	// 8 */
/*   "FreqOn",	// 9 */
/*   "FreqSus",	// 10  */
/*   "FreqAtkSlew",	// 11 */
/*   "FreqDecSlew",	// 12 */
/*   "FreqRelSlew",	// 13 */
/*   "FreqSusDur",	// 14 */
/*   "RqOff",		// 15 */
/*   "RqOn",		// 16 */
/*   "RqSus",		// 17 */
/*   "RqAtkSlew",	// 18 */
/*   "RqDecSlew",	// 19 */
/*   "RqRelSlew",	// 20 */
/*   "RqSusDur",	// 21 */
/*   "Low",		// 22 */
/*   "High",		// 23 */
/*   "Band",		// 24 */
/*   "Notch",		// 25 */
/*   "SvfPre",	// 26 */
/*   "FreqEnv",	// 27 */
/*   "RqEnv",		// 28 */
/* }; */


/* void fill_param_desc(ParamDesc* desc) { */
/*   u32 i, j, k; */
/*   char buf[32] = ""; */
/*   const char * numstrs[] = { "0", "1", "2", "3" }; */
/*   //char strbuf[32]; */
/*   k = 0; */
/*   //  for(i=0; i<eParamNumParams; i++) { */
/*   for(i=0; i<DRUMSYN_NVOICES; i++) { */
/*     for(j=0; j < PARAM_VOICE_NPARAMS; j++) { */
/*     /// ... print to string */
/*       strcpy(buf, paramStrings[j]); */
/*       strcat(buf, numstrs[i]); */
/*       strcpy(desc[k].label, buf); */
/*       desc[k].type = eParamTypeFract; */
/*       desc[k].min = 0; */
/*       desc[k].max = FR32_MAX; */
/*       k++; */
/*     } */
/*   } */
/* } */
