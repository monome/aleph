
#ifndef __PLATFORM_PICOLISP_H__
#define __PLATFORM_PICOLISP_H__

#include "picolisp_mod.h"

// Define all picoLisp specific platform
// components here.

#define PICOLISP_TARGET_SPECIFIC_PROTOS

#define PICOLISP_PROTO(fname)\
  any fname(any x);

#define PICOLISP_TARGET_SPECIFIC_LIBS

#endif // #ifndef __PLATFORM_PICOLISP_H__
