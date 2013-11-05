#ifndef _FONT_H_
#define _FONT_H_

//#include "compiler.h"
#include "types.h"

#define FONT_CHARW 6
#define FONT_CHARH 8
#define FONT_CHARH_1 7
// glyph table doesn't include the initial non-ascii chars
#define FONT_ASCII_OFFSET 0x20

//---------------------------
//---- variables

typedef struct _glyph {
  U8    first     ;       // column inset from left side in proportional mode
  U8    last      ;       // column inset from left side in proportional mode
  U8    data[FONT_CHARW]; // column data 
} glyph_t;

extern const glyph_t font_data[];

extern const U32 font_nglyphs;

//-------------------------------
//--- functions

// render a single glyph to a buffer, rastering
// given pointer, row length, foreground, background
// returns updated pointer
extern u8* font_glyph(char ch, u8* buf, u8 w, u8 a, u8 b);
// same as font_glyph, double size
extern u8* font_glyph_big(char ch, u8* buf, u8 w, u8 a, u8 b);
// render a string of packed glyphs to a buffer
extern u8* font_string(const char* str, u8* buf, u32 size, u8 w, u8 a, u8 b);
// same as font_string, double size
extern u8* font_string_big(const char* str, u8* buf, u32 size, u8 w, u8 a, u8 b);

#endif // header guard
