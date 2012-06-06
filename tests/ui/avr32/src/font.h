#ifndef _FONT_H_
#define _FONT_H_

// number of characters
#define FONT_NCHAR 128
// characer with
#define FONT_CHARW 8
// character height
#define FONT_CHARH 8
// total bytes: NCHAR * CHARH
#define FONT_NBYTES 896
// column mask: 1 << FONT_CHARW
#define FONT_COLMASK 0x80
// font array
extern const U8 rom_font[FONT_NBYTES];

#endif // header guard
