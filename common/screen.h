/* screen.h
   ui test
   aleph
*/

#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "types.h"

#ifdef ARCH_AVR32
#include "font.h"
#endif

//-----------------------------
//---- defines
// number of columns
#define NCOLS 128
#define NCOLS_2 64
// number of rows
#define NROWS 64
#define NROWS_2 32
// pixels in graphics ram 
#define GRAM_PIX 8192 // ncols * nrows
// bytes in graphics RAM
#define GRAM_BYTES 4096 // 2 pixels per byte
// how many lines of text
//#define CHAR_ROWS 8  // NROWS / FONT_CHARH

//#define CHAR_COLS 16 // NCOLS / FONT_CHARW
// what? thats fucked up...
#define CHAR_COLS 32

// get y pixel coordinate of a given line of text
#define SCREEN_LINE(n) FONT_CHARH * (n)

// send startup commands
void init_oled(void);
// draw a pixel
void screen_pixel(U16 x, U16 y, U8 a);
//// all the character-drawing routines now return last used column position
// draw a single character glyph with fixed spacing
U8 screen_char(U16 x, U16 y, char c, U8 a);
// draw a single glyph with proportional spacing
U8 screen_char_squeeze(U16 x, U16 y, char c, U8 a);
// draw a string with fixed spacing
U8 screen_string(U16 x, U16 y, char* str, U8 a);
// draw a string with proportional spacing
U8 screen_string_squeeze(U16 x, U16 y, char* str, U8 a);
// draw a line to the end with fixed spacing
U8 screen_line(U16 x, U16 y, char* str, U8 a);
// print a formatted integer
U8 screen_int(U16 x, U16 y, S32 i, U8 a);
// print a formatted float
U8 screen_float(U16 x, U16 y, F32 f, U8 a);
// write screen buffer to OLED graphics RAM
void screen_refresh(void);
// fill a line with blank space from x to end
void screen_blank(U16 x, U16 y);
// highlight a region
void screen_hl(U16 x, U16 y, U8 a, S8 len);
// get the value at a pixel
U8 screen_get_pixel(U8 x, U8 y);

#endif // header guard
