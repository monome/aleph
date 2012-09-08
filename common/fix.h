/* fix.h
 * aleph/common
 * 
 * some extra utilities for fixed point formatting
 */

#ifndef _ALEPH_FIX_H_
#define _ALEPH_FIX_H_

#include "fix16.h"

#define FIX_DIG_LO    4 // digits after decimal
#define FIX_DIG_HI    5 // digits before decimal
#define FIX_DIG_TOTAL 11 // lo + hi + '.' + '-'

// print to a buffer
void print_fix16(char* buf , fix16_t x);
// whole-part integer to ascii, right-justified, fixed-length
void itoa_whole(int val, char* buf, int len);
// fractional part to ascii, fixed length
void itoa_fract(int val, char* buf);

#endif
