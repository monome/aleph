/* ui,c
  * bees
  * aleph
  *
  * lowest-level user interface routines
  * (screen drawing, key reading, etc)
  *
  * this test version of the layer runs on linux with lncurses
  */

#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "../common/types.h"
#include "menu.h"
#include "ui.h"

//---- defines
#define KEY_QUIT        'q'
#define KEY_EDIT        '`'
#define KEY_ENC_A_UP    '='
#define KEY_ENC_A_DOWN  '-'
#define KEY_ENC_B_UP    ']'
#define KEY_ENC_B_DOWN  '['
#define KEY_ENC_C_UP    '\''
#define KEY_ENC_C_DOWN  ';'
#define KEY_ENC_D_UP    '/'
#define KEY_ENC_D_DOWN  '.'
#define KEY_FN_A_UP     'a'
#define KEY_FN_A_DOWN   'z'
#define KEY_FN_B_UP     's'
#define KEY_FN_B_DOWN   'x'
#define KEY_FN_C_UP     'd'
#define KEY_FN_C_DOWN   'c'
#define KEY_FN_D_UP     'f'
#define KEY_FN_D_DOWN   'v'

// static vars
static char printBuf[SCREEN_W];
static const char hlChars[7] = {' ', '.', '-','~','=','*', '#'};

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
  case KEY_EDIT:
    menu_handleKey(eKeyEdit);
    break;
    /// "encoders"
  case KEY_ENC_A_UP:
    menu_handleKey(eKeyEncUpA);
    break;
  case KEY_ENC_A_DOWN:
    menu_handleKey(eKeyEncDownA);
    break;
  case KEY_ENC_B_UP:
    menu_handleKey(eKeyEncUpB);
    break;
  case KEY_ENC_B_DOWN:
    menu_handleKey(eKeyEncDownB);
    break;
  case KEY_ENC_C_UP:
    menu_handleKey(eKeyEncUpC);
    break;
  case KEY_ENC_C_DOWN:
    menu_handleKey(eKeyEncDownC);
    break;
  case KEY_ENC_D_UP:
    menu_handleKey(eKeyEncUpD);
    break;
  case KEY_ENC_D_DOWN:
    menu_handleKey(eKeyEncDownD);
    break;
    // function keys
  case KEY_FN_A_DOWN:
    menu_handleKey(eKeyFnDownA);
    break;
  case KEY_FN_A_UP:
    menu_handleKey(eKeyFnUpA);
    break;
  case KEY_FN_B_DOWN:
    menu_handleKey(eKeyFnDownB);
    break;
  case KEY_FN_B_UP:
    menu_handleKey(eKeyFnUpB);
    break;
  case KEY_FN_C_DOWN:
    menu_handleKey(eKeyFnDownC);
    break;
  case KEY_FN_C_UP:
    menu_handleKey(eKeyFnUpC);
    break;
  case KEY_FN_D_DOWN:
    menu_handleKey(eKeyFnDownD);
    break;
  case KEY_FN_D_UP:
    menu_handleKey(eKeyFnUpD);
    break;
  default:
    break;
  }
  return run;
}

// print a line of text
/*
void ui_println(U8 y, const char* str) {
 mvprintw(y, 0, str);
 refresh();
}
*/

// print some characters of text
void ui_print(U8 y, U8 x, const char* str, u8 hl) {
  u8 i;
  for(i=0; i<SCREEN_W; i++) {
    printBuf[i] = ' ';
  }
  strcpy(printBuf, str);
  // fill spaces in output string according to hilight
  i=0; 
  
  for(i=0; i<SCREEN_W; i++) {   
    if ((printBuf[i] == ' ') || (printBuf[i] == 0)) {
      printBuf[i] = hlChars[hl];
    }
  }
  mvprintw(y, x, printBuf);
  refresh();
}
