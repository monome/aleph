/*
  font:
  ume gothic monospace O 5
  18px, 22x18 bounds
 */

#ifndef _FONT_UME_TGO5_18_H_
#define _FONT_UME_TGO5_18_H_

#define FONT_UME_TGO5_18_W 9
#define FONT_UME_TGO5_18_H 19

#define FONT_UME_TG05_18_NGLYPHS 93

// datatype for font glyph.
typedef union {
    struct {
        char w;
        char h;
        char data[171];
    } glyph;
    char raw[173];
} ume_tgo5_18_glyph;

// table of glyphs in this font
extern const ume_tgo5_18_glyph font_ume_tgo5_18[FONT_UME_TG05_18_NGLYPHS];

#endif
