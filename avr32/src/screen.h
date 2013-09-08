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

// bytes in one row (pix width / 2)
#define SCREEN_ROW_BYTES 	64
#define SCREEN_ROW_BYTES_1 	63
// bytes in one column
#define SCREEN_COL_BYTES 	64
#define SCREEN_COL_BYTES_1	63

// bytes in graphics RAM
#define GRAM_BYTES  4096 // 2 pixels per byte
#define GRAM_BYTES_1  4095
//-----------------------------
//----  functions

// send startup commands
extern void init_oled(void);
// draw data at given rectangle
extern void screen_draw_region(u8 x, u8 y, u8 w, u8 h, u8* data);
// draw the whole screen
// extern void screen_draw_full(u8 x, u8 y, u8 w, u8 h, u8* data);
// clear the whole screen
extern void screen_clear(void);

// show startup screen
void screen_startup(void);

#endif // header guard
