#ifndef _ALEPH_AVR32_FONT_AA_H_
#define _ALEPH_AVR32_FONT_AA_H_

#include "types.h"


// render an anti-aliased (4-bit) glyph to a buffer
// arguments are character, buffer, target row size, invert flag
extern u8* font_glyph_aa(char ch, u8* buf, u8 w, u8 invert);

// render a string of ant-aliased glyphs to a buffer
extern u8* font_string_aa(const char* str, u8* buf, u32 size, u8 w, u8 invert);

#endif
