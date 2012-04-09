/* ui.h
 * bees
 * aleph
 *
 * public interface for low-level UI routines
 * (screen drawing, key reading)
 */

#ifndef _UI_H_
#define _UI_H_

#include "../common/types.h"
#include "ncurses.h"

//---- defines
// screen width in characters
#define SCREEN_W 32
// screen height in characters
#define SCREEN_H   8
#define SCREEN_H_1 7

//---- external function declarations
// initialize low-level user interface (screen, keys)

//---- public functions
// init
void ui_init(void);
// de-init
void ui_deinit(void);
// loop over user input
U8 ui_loop(void);
// print a line of text
void ui_println(U8 y, char* str);
// print some characters of text
void ui_print(U8 y, U8 x, char* str);

#endif // header guard
