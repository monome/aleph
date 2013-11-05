/* font: 
   deja vu monospave bold, 24px bounds, (30px font height i believe)

 */

// for sanity's sake, width and height are preserved from the .xpm file.
// let's hardcode these rather than actually accessing them from memory though.

#ifndef _FONT_DEJAVU_24_H_
#define _FONT_DEJAVU_24_H_

#define FONT_DEJAVU_W 18
#define FONT_DEJAVU_H 24


// datatype for font glyph.
typedef union {
  struct { 
    char w;
    char h;
    char data[432];
  } glyph;
  char raw[434];
} dejavu_24_glyph;

// table of glyphs in this font
extern const dejavu_24_glyph font_dejavu_24_numerals[11];

#endif
