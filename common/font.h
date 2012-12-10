#ifndef _FONT_H_
#define _FONT_H_

//#include "compiler.h"
#include "types.h"

#define FONT_CHARW 6
#define FONT_CHARH 8
#define FONT_CHARH_1 7
// glyph table doesn't include the initial non-ascii chars
#define FONT_ASCII_OFFSET 0x20

typedef struct _glyph {
  U8    first     ;       // column inset from left side in proportional mode
  U8    last      ;       // column inset from left side in proportional mode
  U8    data[FONT_CHARW]; // column data 
} glyph_t;

extern const glyph_t font_data[];

extern const U32 font_nglyphs;

#endif // header guard
