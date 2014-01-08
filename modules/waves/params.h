
#ifndef _ALEPH_MODULE_WAVES_PARAMS_H_
#define _ALEPH_MODULE_WAVES_PARAMS_H_

//---------- defines
// ranges and radix
// ranges are in 16.16
// radix should be minimal bits to accommodate entire integer range.
/* #define OSC_FREQ_MIN 0x040000      // 4 hz */
/* #define OSC_FREQ_MAX 0x40000000    // 16384 hz */
/* #define OSC_FREQ_RADIX 15 */

#define RATIO_MIN 0x4000     // 1/4
#define RATIO_MAX 0x40000    // 4
#define RATIO_RADIX 3

#define SMOOTH_FREQ_MIN 0x2000 // 1/8
#define SMOOTH_FREQ_MAX 0x400000 // 64
#define SMOOTH_FREQ_RADIX 7

// svf cutoff
// filter cutoff frequency
#define PARAM_FREQ_MAX 0x1F400000;  // fixme

// filter reciprocal of Q
#define PARAM_RQ_MIN 0x00000000
#define PARAM_RQ_MAX 0x7fffffff

#define NUM_PARAMS eParamNumParams

extern void fill_param_desc(void);

#endif // h guard
