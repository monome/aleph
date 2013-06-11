/* screen.h
   ui test
   aleph
*/

#ifndef _SCREEN_H_
#define _SCREEN_H_

#if ARCH_AVR32
#include "compiler.h"
#endif
#include "fix.h"
#include "types.h"

#ifdef ARCH_AVR32
#include "font.h"
#endif

//-----------------------------
//---- defines
// number of columns
#define NCOLS       128
#define NCOLS_1     127
#define NCOLS__2     64
#define COLS_LSHIFT  6
// number of rows
#define NROWS       64
#define NROWS__2     32
// pixels in graphics ram 
#define GRAM_PIX    8192 // ncols * nrows
// bytes in graphics RAM
#define GRAM_BYTES  4096 // 2 pixels per byte
#define GRAM_BYTES_1  4095
// how many lines of text
#define CHAR_ROWS   8  // NROWS / FONT_CHARH
#define CHAR_ROWS_1 7
#define CHAR_ROWS_2 6
// arbitrary maximum for proportionally spaced line drawing
#define CHAR_COLS   50
#define CHAR_COLS_1 49
// center row of text
#define SCREEN_ROW_CENTER 3

// get y pixel coordinate of a given line of text
// #define SCREEN_LINE(n) FONT_CHARH * (n)

//-----------------------------
//----  variables
// get y pixel coordinate of a given line of text
// cheating with magic numbers instead of FONT_CHARH
// for const-ability
extern const u8 kScreenLines[];


//-----------------------------
//----  functions

// send startup commands
extern void init_oled(void);
// clear the screen
extern void screen_clear(void);
// draw a pixel
extern void screen_pixel(U16 x, U16 y, U8 a);
//// all the character-drawing routines now return last used column position
// draw a single character glyph with fixed spacing
extern U8 screen_char_fixed(U16 x, U16 y, char c, U8 a);
// draw a single glyph with proportional spacing
extern U8 screen_char_squeeze(U16 x, U16 y, char c, U8 a);
// draw a string with fixed spacing
extern U8 screen_string_fixed(U16 x, U16 y, char* str, U8 a);
// draw a string with proportional spacing
extern U8 screen_string_squeeze(U16 x, U16 y, char* str, U8 a);
// draw a string (default)
extern U8 screen_string(U16 x, U16 y, char* str, U8 a);
// draw a line to the end with default spacing
extern U8 screen_line(U16 x, U16 y, char* str, U8 a);
// print a formatted integer
extern U8 screen_int(U16 x, U16 y, S16 i, U8 a);
// print a formatted float
//U8 screen_float(U16 x, U16 y, F32 f, U8 a);
// print a formatted fix16
extern U8 screen_fix(U16 x, U16 y, fix16_t v, U8 a);
// write screen buffer to OLED graphics RAM
extern void screen_refresh(void);
// fill a line with blank space from x to end
extern void screen_blank_line(U16 x, U16 y);
// highlight part or all of a line
extern void screen_hl_line(U16 x, U16 y, U8 a);
// get the value at a pixel
extern U8 screen_get_pixel(U8 x, U8 y);
// fill with a test pattern of characters
extern void screen_test_fill(void);
// blank the whole screen
extern void screen_blank(void);
// draw a rectangular frame
extern void screen_frame(u16 x, u16 y, u16 width, u16 height, u8 framesize, u8 a);


#endif // header guard
