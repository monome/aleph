#include "simple_string.h"

#define SMPSTR_MAX_LEN 128

// copy with length arg (-1 == until null terminator)
void str_copy(const char* src, char * dst, s8 len) {
  u8 i;
  if(len < 0) {
    while(i<SMPSTR_MAX_LEN) {
      if(src[i] == 0) {
	break;
      }
      dst[i] = src[i];
    }
  } else {
    for(i=0; i<len; i++) {
      dst[i] = src[i];
    }
  }
}

