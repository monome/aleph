/* ui,c
  * bees
  * aleph
  *
  * lowest-level user interface routines
  * (screen drawing, key reading, etc)
  */

#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "../common/types.h"
#include "menu.h"
#include "ui.h"

//---- defines
#define KEY_QUIT        'q'
#define KEY_ENC_A_UP    '='
#define KEY_ENC_A_DOWN  '-'
#define KEY_ENC_B_UP    ']'
#define KEY_ENC_B_DOWN  '['
#define KEY_ENC_C_UP    '\''
#define KEY_ENC_C_DOWN  ';'
#define KEY_ENC_D_UP    '/'
#define KEY_ENC_D_DOWN  '.'
#define KEY_FN_A        'z'
#define KEY_FN_B        'x'
#define KEY_FN_C        'c'
#define KEY_FN_D        'v'


//---- external function definitions

// initialize low-level user interface (screen, keys)
void ui_init(void) {
  // using ncurses in the dumbest way (no fields, no windows)
  // (dont want to rely on it overly much)
  initscr();             // start curses mode
  raw();                 // no line buffering
  keypad(stdscr, TRUE);   // capture function keys, etc
  noecho();
  curs_set(0); // invisible cursor
}

// cleanup UI
void ui_deinit(void) {
  erase();
  endwin();
}

// loop over user input events
U8 ui_loop(void) {
  // use standard character input
  static U16 cmd;
  static U8 run, ch;  
  cmd = getchar();
  
  run = 1;

  switch(cmd) {  
  case KEY_QUIT:
    mvprintw(0, 1, "really quit? (y)");
    refresh();      
    ch = getch();
    if ((ch == 'y') || (ch == 'Y')) {
      run = 0;
    }
    break;
  case KEY_ENC_A_UP:
    menu_handleKey(eKeyUpA);
    break;
  case KEY_ENC_A_DOWN:
    menu_handleKey(eKeyDownA);
    break;
  case KEY_ENC_B_UP:
    menu_handleKey(eKeyUpB);
    break;
  case KEY_ENC_B_DOWN:
    menu_handleKey(eKeyDownB);
    break;
  case KEY_ENC_C_UP:
    menu_handleKey(eKeyUpC);
    break;
  case KEY_ENC_C_DOWN:
    menu_handleKey(eKeyDownC);
    break;
  case KEY_ENC_D_UP:
    menu_handleKey(eKeyUpD);
    break;
  case KEY_ENC_D_DOWN:
    menu_handleKey(eKeyDownD);
    break;
  case KEY_FN_A:
    menu_handleKey(eKeyFnA);
    break;
  case KEY_FN_B:
    menu_handleKey(eKeyFnB);
    break;
  case KEY_FN_C:
    menu_handleKey(eKeyFnC);
    break;
  case KEY_FN_D:
    menu_handleKey(eKeyFnD);
    break;
  default:
    break;
  }
  return run;
}

// print a line of text
void ui_println(U8 y, const char* str) {
 mvprintw(y, 0, str);
 refresh();
}

// print some characters of text
void ui_print(U8 y, U8 x, const char* str) {
  mvprintw(y, x, str);
  refresh();
}
