/* oled.h
   ui test
   aleph
*/

#ifndef _OLED_H_
#define _OLED_H_

#include "compiler.h"

#define NPIXELS 8192 // 128 * 64

void init_oled(void);
void oled_draw_pixel_raw(U16 pos, U8 a);
void oled_draw_pixel(U16 x, U16 y, U8 a);
void oled_draw_char(U16 x, U16 y, U8 c, U8 a);
void oled_draw_string(U16 x, U16 y, U8* str, U8 a);
void oled_refresh(void);

#endif // header guard
