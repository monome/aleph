#ifndef _FONT_H_
#define _FONT_H_

//#include "compiler.h"
//#include "fonts/dejavu_numerals_24.h"
#include "fonts/ume_tgo5_18.h"

#include "region.h"
#include "types.h"

// small bitfield system font
#define FONT_CHARW 6
#define FONT_CHARH 8
#define FONT_CHARH_1 7
// glyph table doesn't include the initial non-ascii chars
#define FONT_ASCII_OFFSET 0x20

#define FONT_AA font_ume_tgo5_18
#define FONT_AA_CHARW 	FONT_UME_TGO5_18_W
#define FONT_AA_CHARH 	FONT_UME_TGO5_18_H

//---------------------------
//---- variables

typedef struct _glyph {
  u8    first     ;       // column inset from left side in proportional mode
  u8    last      ;       // column inset from left side in proportional mode
  u8    data[FONT_CHARW]; // column data 
} glyph_t;

extern const glyph_t font_data[];

extern const u32 font_nglyphs;

//-------------------------------
//--- functions

// render a single glyph to a buffer, rastering
// given pointer, row length, foreground, background
// returns count of columns
extern u8 font_glyph(char ch, u8* buf, u8 w, u8 a, u8 b);
// fixed-width variant
extern u8 font_glyph_fixed(char ch, u8* buf, u8 w, u8 a, u8 b);
// same as font_glyph, double size
/// -- -fixme: these still return updated buf pointers... no width checking
extern u8* font_glyph_big(char ch, u8* buf, u8 w, u8 a, u8 b);
// same as font_glyph, 4x size
extern u8* font_glyph_bigbig(char ch, u8* buf, u8 w, u8 a, u8 b);
// render a string of packed glyphs to a buffer
extern u8* font_string(const char* str, u8* buf, u32 size, u8 w, u8 a, u8 b);
// same as font_string, double size
extern u8* font_string_big(const char* str, u8* buf, u32 size, u8 w, u8 a, u8 b);
// same as font_string, 4x size
extern u8* font_string_bigbig(const char* str, u8* buf, u32 size, u8 w, u8 a, u8 b);


// render a font string to a region.
// this allows for smarter bounds handling
extern void font_string_region_wrap(region* reg, const char* str, u8 x, u8 y, u8 fg, u8 bg);
extern void font_string_region_clip(region* reg, const char* str, u8 x, u8 y, u8 fg, u8 bg);

///--- anti-aliased

// render an anti-aliased (4-bit) glyph to a buffer
// arguments are character, buffer, target row size, invert flag
extern u8* font_glyph_aa(char ch, u8* buf, u8 w, u8 invert);

// render a string of ant-aliased glyphs to a buffer
extern u8* font_string_aa(const char* str, u8* buf, u32 size, u8 w, u8 invert);

#endif // header guard
