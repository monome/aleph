#ifndef _ALEPH_MIX_PARAMS_H_
#define _ALEPH_MIX_PARAMS_H_

#include "param_common.h"

// define some constants here for the param descriptor code
// here, all the parameter ranges are pretty simple.
#define PARAM_AMP_MAX 	0x7fffffff
#define PARAM_CV_MAX 	0x7fffffff
#define PARAM_SLEW_MAX 	0x7fffffff

#define PARAM_SECONDS_MAX 0x003c0000
#define PARAM_SECONDS_RADIX 7

// something pretty fast, but noticeable
#define PARAM_SLEW_DEFAULT 	0x7ffecccc

// enumerate parameters
// the order defined here must be matched in the descriptor !
enum params {
    
    eParam0,
    eParam1,
    eParam2,
    eParam3,
    eParam4,
    eParam5,
    eParam6,
    eParam7,
    eParam8,
    eParam9,
    eParam10,
    eParam11,
    eParam12,
    eParam13,
    eParam14,
    eParam15,
    eParam16,
    eParam17,
    eParam18,
    eParam19,

  eParamNumParams
};

// this is only defined by the descriptor helper program.
extern void fill_param_desc(ParamDesc* desc);

#endif // header guard 
// EOF
