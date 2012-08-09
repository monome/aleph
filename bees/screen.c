/* screen,c
  * bees
  * aleph
  *
  * lowest-level user interface routines
  * (screen drawing, key reading, etc)
  *
  * this version of the layer runs on linux with lncurses
  * it should only be compiled/linked from a linsux make session
  * (e.g., lives in aleph/bees, not aleph/avr32)
  */


#ifndef ARCH_LINUX
#error using the wrong screen.c (linux/ncurses)
#endif

#include <stdio.h>
#include <string.h>
#include "ncurses.h"


#include "screen.h"


#include "types.h"
#include "menu.h"
// #include "ui.h"

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

//---- static vars

// line buffer
static char linebuf[CHAR_COLS]; // line buffer

//---- external function definitions

// initialize low-level user interface (screen, keys)
void ui_init(void) {
  u8 i;

  initscr();             // start curses mode
  raw();                 // no line buffering
  keypad(stdscr, TRUE);   // capture function keys, etc
  noecho();
  curs_set(0); // invisible cursor
  
  start_color();

  // weird hacky color definitions to make an 8-point grayscale kinda
  if(0) { // (can_change_color()) {
    for(i=0; i<8; i++) {
      init_color(i, i*140, i*140, i*140 );
    }
    // now use pair(i), i in [0, 7], for background color
    for(i=0; i<8; i++) {
      init_pair(i+1, i, (i + 4) % 7);
    }
  } else {

    

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);
    init_pair(4, COLOR_WHITE, COLOR_BLUE);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_RED);
    //init_pair(7, COLOR_CYAN, COLOR_BLACK);
    //init_pair(8, COLOR_CYAN, COLOR_MAGENTA);
    
  }
}

// cleanup UI
void ui_deinit(void) {
  erase();
  endwin();
}

// loop over user input events
u8 ui_loop(void) {
  // use standard character input
  static u16 cmd;
  static u8 run, ch;  
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
void ui_println(u8 y, const char* str) {
 mvprintw(y, 0, str);
 refresh();
}
*/

// print some characters of text
u8 screen_string(u16 x, u16 y, char* str, u8 hl) {
  s8 l = 7 - hl;      // 8-point level of background
  if (l < 0) {l = 0;} 
  if (l > 7) {l = 7;}
  attron(COLOR_PAIR(l));
  mvprintw(y, x, str);
  refresh();
  return strlen(str);
}

// print and blank line to end
u8 screen_line(u16 x, u16 y, char* str, u8 hl) {
  //  s8 l = 7 - hl;      // 8-point level of background
  u8 i=0;
  u8 len = strlen(str);
  
  //if (l < 0) {l = 0;} 
  //if (l > 7) {l = 7;}
 
  for(i=0; i<CHAR_COLS; i++) {
    if(i<len) { 
      linebuf[i] = str[i];
    } else {
      linebuf[i] = ' ';
    }
  }

  attron(COLOR_PAIR(hl));
  mvprintw(y, x, linebuf);
  refresh();
  
  return CHAR_COLS;
}
