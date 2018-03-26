/*
    params.c
    parameter descriptor fields.
*/

#include <string.h>
#include "module.h"
#include "params.h" 

extern void fill_param_desc(ParamDesc* desc) {
  
    strcpy(desc[eParam0].label, "level 0");
    desc[eParam0].type = eParamTypeAmp;
    desc[eParam0].min = 0x00000000;
    desc[eParam0].max = PARAM_AMP_MAX;
    desc[eParam0].radix = 16;

    strcpy(desc[eParam1].label, "start 0");
    desc[eParam1].type = eParamTypeFix;
    desc[eParam1].min = 0x00000000;
    desc[eParam1].max = PARAM_SECONDS_MAX;
    desc[eParam1].radix = PARAM_SECONDS_RADIX;
    
    strcpy(desc[eParam2].label, "loop 0");
    desc[eParam2].type = eParamTypeFix;
    desc[eParam2].min = 0x00000000;
    desc[eParam2].max = PARAM_SECONDS_MAX;
    desc[eParam2].radix = PARAM_SECONDS_RADIX;

    strcpy(desc[eParam3].label, "direction 0");
    desc[eParam3].type = eParamTypeBool;
    desc[eParam3].min = 0;
    desc[eParam3].max = 1;
    desc[eParam3].radix = 1;

    strcpy(desc[eParam4].label, "trig 0");
    desc[eParam4].type = eParamTypeBool;
    desc[eParam4].min = 0;
    desc[eParam4].max = 1;
    desc[eParam4].radix = 1;

    strcpy(desc[eParam5].label, "level 1");
    desc[eParam5].type = eParamTypeAmp;
    desc[eParam5].min = 0x00000000;
    desc[eParam5].max = PARAM_AMP_MAX;
    desc[eParam5].radix = 16;
    
    strcpy(desc[eParam6].label, "start 1");
    desc[eParam6].type = eParamTypeFix;
    desc[eParam6].min = 0x00000000;
    desc[eParam6].max = PARAM_SECONDS_MAX;
    desc[eParam6].radix = PARAM_SECONDS_RADIX;
    
    strcpy(desc[eParam7].label, "loop 1");
    desc[eParam7].type = eParamTypeFix;
    desc[eParam7].min = 0x00000000;
    desc[eParam7].max = PARAM_SECONDS_MAX;
    desc[eParam7].radix = PARAM_SECONDS_RADIX;
    
    strcpy(desc[eParam8].label, "direction 1");
    desc[eParam8].type = eParamTypeBool;
    desc[eParam8].min = 0;
    desc[eParam8].max = 1;
    desc[eParam8].radix = 1;
    
    strcpy(desc[eParam9].label, "trig 1");
    desc[eParam9].type = eParamTypeBool;
    desc[eParam9].min = 0;
    desc[eParam9].max = 1;
    desc[eParam9].radix = 1;

    strcpy(desc[eParam10].label, "level 2");
    desc[eParam10].type = eParamTypeAmp;
    desc[eParam10].min = 0x00000000;
    desc[eParam10].max = PARAM_AMP_MAX;
    desc[eParam10].radix = 16;
    
    strcpy(desc[eParam11].label, "start 2");
    desc[eParam11].type = eParamTypeFix;
    desc[eParam11].min = 0x00000000;
    desc[eParam11].max = PARAM_SECONDS_MAX;
    desc[eParam11].radix = PARAM_SECONDS_RADIX;
    
    strcpy(desc[eParam12].label, "loop 2");
    desc[eParam12].type = eParamTypeFix;
    desc[eParam12].min = 0x00000000;
    desc[eParam12].max = PARAM_SECONDS_MAX;
    desc[eParam12].radix = PARAM_SECONDS_RADIX;
    
    strcpy(desc[eParam13].label, "direction 2");
    desc[eParam13].type = eParamTypeBool;
    desc[eParam13].min = 0;
    desc[eParam13].max = 1;
    desc[eParam13].radix = 1;
    
    strcpy(desc[eParam14].label, "trig 2");
    desc[eParam14].type = eParamTypeBool;
    desc[eParam14].min = 0;
    desc[eParam14].max = 1;
    desc[eParam14].radix = 1;
    
    strcpy(desc[eParam15].label, "level 3");
    desc[eParam15].type = eParamTypeAmp;
    desc[eParam15].min = 0x00000000;
    desc[eParam15].max = PARAM_AMP_MAX;
    desc[eParam15].radix = 16;
    
    strcpy(desc[eParam16].label, "start 3");
    desc[eParam16].type = eParamTypeFix;
    desc[eParam16].min = 0x00000000;
    desc[eParam16].max = PARAM_SECONDS_MAX;
    desc[eParam16].radix = PARAM_SECONDS_RADIX;
    
    strcpy(desc[eParam17].label, "loop 3");
    desc[eParam17].type = eParamTypeFix;
    desc[eParam17].min = 0x00000000;
    desc[eParam17].max = PARAM_SECONDS_MAX;
    desc[eParam17].radix = PARAM_SECONDS_RADIX;
    
    strcpy(desc[eParam18].label, "direction 3");
    desc[eParam18].type = eParamTypeBool;
    desc[eParam18].min = 0;
    desc[eParam18].max = 1;
    desc[eParam18].radix = 1;
    
    strcpy(desc[eParam19].label, "trig 3");
    desc[eParam19].type = eParamTypeBool;
    desc[eParam19].min = 0;
    desc[eParam19].max = 1;
    desc[eParam19].radix = 1;
}

// EOF
