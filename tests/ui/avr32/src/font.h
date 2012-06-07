#ifndef _FONT_H_
#define _FONT_H_

#define FONT_CHARW 6
#define FONT_CHARH 8
// glyph table doesn't include the initial non-ascii chars
#define FONT_ASCII_OFFSET 0x20

typedef struct _glyph {
  // U32   code  : 20; /* Unicode */
  // U8    split :  1; /* split marker */
  // U8    attr  :  2; /* glyph attributes */
  U8    first :  2; /* first column to be displayed in proportional mode */
  U8    last  :  3; /* last  column to be displayed in proportional mode */
  U8    data[FONT_CHARW]; /* column data */
} glyph;

extern const glyph font_data[];
//extern const U32 const getNumGlyphs(void) { return font_nglyphs; }

#endif // header guard
