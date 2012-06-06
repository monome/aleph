/* oled.h
   ui test
   aleph
*/

#ifndef _OLED_H_
#define _OLED_H_

#include "compiler.h"
#include "font.h"

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
#define CHAR_ROWS 8  // NROWS / FONT_CHARH
#define CHAR_COLS 16 // NCOLS / FONT_CHARW

void init_oled(void);
//void oled_draw_screen_raw(U16 pos, U8 data);
void oled_draw_pixel(U16 x, U16 y, U8 a);
void oled_draw_char(U16 x, U16 y, char c, U8 a);
void oled_draw_string(U16 x, U16 y, char* str, U8 a);
void oled_refresh(void);

#endif // header guard
