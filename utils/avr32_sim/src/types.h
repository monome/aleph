#ifndef _ALEPH_SIM_TYPES_H_
#define _ALEPH_SIM_TYPES_H_

#include <stdint.h>
#include "fat_filelib.h"

#ifdef ARCH_AVR32
#include "compiler.h"
#else
#endif

#ifndef true
#define true 1
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef POWER_CTL_PIN
#define POWER_CTL_PIN 0
#endif


typedef uint8_t      uint8;  //!< 8-bit unsigned integer.
//---- basic numerical datatypes

typedef int8_t       s8 ;  //!< 8-bit signed integer.
typedef uint8_t      u8 ;  //!< 8-bit unsigned integer.
typedef int16_t      s16;  //!< 16-bit signed integer.
typedef uint16_t     u16;  //!< 16-bit unsigned integer.
typedef int32_t      s32;  //!< 32-bit signed integer.
typedef uint32_t     u32;  //!< 32-bit unsigned integer.
typedef int64_t      s64;  //!< 64-bit signed integer.
typedef uint64_t     u64;  //!< 64-bit unsigned integer.
typedef float        f32;  //!< 32-bit floating-point number.
typedef double       f64;  //!< 64-bit floating-point number.

typedef s8  S8;  //!< 8-bit signed integer.
typedef u8  U8;  //!< 8-bit unsigned integer.
typedef s16 S16;  //!< 16-bit signed integer.
typedef u16 U16;  //!< 16-bit unsigned integer.
typedef s32 S32;  //!< 32-bit signed integer.
typedef u32 U32;  //!< 32-bit unsigned integer.
typedef s64 S64;  //!< 64-bit signed integer.
typedef u64 U64;  //!< 64-bit unsigned integer.
typedef f32 F32;  //!< 32-bit floating-point number.
typedef f64 F64;  //!< 64-bit floating-point number.

// fract
typedef int32_t fix16;
typedef int32_t  fract32;

typedef unsigned char bool;

#endif // header guard
