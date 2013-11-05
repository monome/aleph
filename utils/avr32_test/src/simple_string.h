#ifndef _SIMPLE_STRING_H_
#define _SIMPLE_STRING_H_

#include "types.h"

// copy whole string
//extern void str_copy(const char* src, char * dst);
// copy with length arg (-1 == until null terminator)
extern void str_copy(char* dst, const char * src, s8 len);

#endif

