#ifndef RICKS_BFIN_EMU
#define RICKS_BFIN_EMU

typedef int fract32;
typedef int s32;
typedef unsigned int u32;
/* #define FR32_MAX 0x7FFFFFFF */
#define FR32_MIN 0x80000000
fract32 mult_fr1x32x32(fract32 x, fract32 y);
fract32 mult_fr1x32x32_cheat(fract32 x, fract32 y);
fract32 sub_fr1x32(fract32 x, fract32 y);
fract32 add_fr1x32(fract32 x, fract32 y);

#endif
