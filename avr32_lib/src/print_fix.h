/* fix.h
 * aleph/common
 * 
 * some extra utilities for fixed point string formatting
 */

#ifndef _ALEPH_FIX_H_
#define _ALEPH_FIX_H_

#include "fix16.h"

#define FIX_DIG_LO 4 // digits after decimal
#define FIX_DIG_HI 5 // digits before decimal
#define FIX_DIG_ALL 11 // lo + hi + '.' + '-'

// print to a buffer (assumed length is FIX_DIG_ALL)
void print_fix16(char* buf , fix16_t x);
// print integer to buffer, return num digits
int itoa_int(int val, char* buf);

#endif
