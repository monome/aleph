
/* 
   buffer_common.h
   aleph-common

   declare common buffer data types.
   basically, simple descriptors.

 */

#ifndef _ALEPH_COMMON_BUFFER_H_
#define _ALEPH_COMMON_BUFFER_H_

#include "fix.h"
#include "types.h"


// descriptor
typedef struct __attribute__((__packed__)) BufferDescStruct {
  // count of channels
  u32 channels;
  // count of frames
  u32 frames;
  //...???
} BufferDesc;


#endif // header guard
