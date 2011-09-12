/* echo_data.h
 * aleph-blackfin echo module
 *
 * declares external data
 */

#ifndef _ALEPH_BFIN_ECHO_DATA_H_
#define _ALEPH_BFIN_ECHO_DATA_H_

#include "types.h"

#define MODULE_NUM_PARAMS 3

#define ECHO_NUMFRAMES 1048576
#define ECHO_NUMFRAMES_1 104875

// #define ECHO_NUMFRAMES 64 * 1024 * 1024 / 4 - 1024
// #define ECHO_NUMFRAMES_1 ECHO_NUMFRAMES - 1

// test...
//#define ECHO_NUMFRAMES 100
//#define ECHO_NUMFRAMES_1 99

#define ECHO_SAMPLERATE 48000

// echo buffer
extern fract32 echoBuf[];

#endif // header guard
