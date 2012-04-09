

/*
void ui_init(void) {
  state = eMain;
}/* ui,c
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
#define CMD_KEY_QUIT     'q'
#define CMD_KEY_UP       KEY_UP
#define CMD_KEY_DOWN     KEY_DOWN
#define CMD_KEY_LEFT     KEY_LEFT
#define CMD_KEY_RIGHT    KEY_RIGHT

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
  static U8 dum, ch;  
  cmd = getchar();
  
  switch(cmd) {  
  case CMD_KEY_QUIT:
    mvprintw(0, 1, "really quit? (y)");
       
    ch = getch();
    if ((ch == 'y') || (ch == 'Y')) {
      dum = 0;
    } else {
      dum = 1;
    }
    return dum;
    break;
  case CMD_KEY_UP:
    break;
  case CMD_KEY_DOWN:
    break;
  case CMD_KEY_LEFT:
    break;
  case CMD_KEY_RIGHT:
    break;
  }
}

// print a line of text
void ui_println(U8 y, char* str) {
  mvprintw(y, 0, str);
}

// print some characters of text
void ui_print(U8 y, U8 x, char* str) {
  mvprintw(y, x, str);
}

/*
U8 ui_loop(void) {
  U8 cmd, dum;
  switch(state) {
    case eMain:
    printf("main menu: \n(l) list ops, (c) create op, (t) connect, (a) activate input, (q) quit\n");
      cmd = getchar();
      dum = getchar(); // newline
      if (cmd == 'l') {
        print_ops();
        return ui_loop(); // recurse
      }
      else if (cmd == 'c') {
        create_op();
        return ui_loop(); // recurse
      } 
      else if (cmd == 't') {
        connect();
        return ui_loop(); // recurse
      } 
      else if (cmd == 'a') {

        activate();
        return ui_loop(); // recurse
      } 
      else if (cmd == 'q') {
        return 0; // quit
      }
      break;
    default:
      return 0; // whoops, quit
  }
  return 0; // whoops, quit
}
*/
