
/* echo_data.h
 * aleph-blackfin echo module
 *
 * declares external data
 */

#ifndef _ALEPH_BFIN_ECHO_DATA_H_
#define _ALEPH_BFIN_ECHO_DATA_H_

#include "types.h"

#define ECHO_SAMPLERATE 48000

// 30 seconds at 48k
#define ECHO_NUMFRAMES   1440000
#define ECHO_NUMFRAMES_1 1439999
// 4 * frames
#define ECHO_NUMSAMPS    5760000

#define MODULE_NUM_PARAMS 24
// parameter indices
enum {
  eParamMix0  ,
  eParamMix1  ,
  eParamMix2  ,
  eParamMix3  ,
  eParamTime0 ,
  eParamTime1 ,
  eParamTime2 ,
  eParamTime3 ,
  eParamFb00  ,
  eParamFb01  ,
  eParamFb02  ,
  eParamFb03  ,
  eParamFb10  ,
  eParamFb11  ,
  eParamFb12  ,
  eParamFb13  ,
  eParamFb20  ,
  eParamFb21  ,
  eParamFb22  ,
  eParamFb23  ,
  eParamFb30  ,
  eParamFb31  ,
  eParamFb32  ,
  eParamFb33
};

// echo buffer
extern fract32 * echoBuf;

#endif // header guard
