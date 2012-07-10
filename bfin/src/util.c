#include "types.h"
#include "util.h"

// swap some bytes
void byte_swap(u8* pData, u8 nb) {
  u8 tmp[nb];
  u8 i;
  for(i=0; i<nb; i++) {
    tmp[i] = *(pData + i);
  }
  for(i=0; i<nb; i++) {
    *(pData + i) = tmp[nb - i - 1];
  }
}

