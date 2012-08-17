/* ui.h
 * bees
 * aleph
 *
 * public interface for low-level UI routines
 * (screen drawing, key reading)
 */

#ifndef _UI_H_
#define _UI_H_

#include "types.h"

//---- defines
// screen width in characters
#define SCREEN_W 64
// screen height in lines
#define SCREEN_H   16
#define SCREEN_H_1 15
#define SCREEN_H_2 14
// which line to center/highlight for editing
// FIXME: breaking this for avr32
///#define SCREEN_ROW_CENTER 7

//---- external function declarations
// initialize low-level user interface (screen, keys)

//---- public functions
// init
void ui_init(void);
// de-init
void ui_deinit(void);
// loop over user input
u8 ui_loop(void);
// print a line of text
void ui_println(u8 y, const char* str);
// print some characters of text
u16 ui_print(u8 y, u8 x, const char* str, u8 hl);

#endif // header guard
