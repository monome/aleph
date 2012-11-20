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
#include "font.h"
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

//-----------------------------
//---- variables
const U8 kScreenLines[CHAR_ROWS] = { 0, 8, 16, 24, 32, 40, 48, 56 };

// pixel buffer
static U8 screen[GRAM_BYTES];
static U8 text[CHAR_ROWS][CHAR_COLS];

static char linebuf[CHAR_COLS]; // line buffer

//---- external function definitions

// initialize low-level user interface (screen, keys)
void ui_init(void) {
  //u8 i;

  initscr();             // start curses mode
  raw();                 // no line buffering
  keypad(stdscr, TRUE);   // capture function keys, etc
  noecho();
  curs_set(0); // invisible cursor
  
  //  start_color();

  /*
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
  */

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

////////////////////
// drawing routines

// zero one column in one glyph
static void zero_col(U16 x, U16 y);
static void zero_col(U16 x, U16 y) {
  static U8 i;
  for(i=0; i<FONT_CHARH; i++) {
    screen_pixel(x, y+i, 0);
  }
}

// draw a single pixel
void screen_pixel(U16 x, U16 y, U8 a) {
  static U32 pos;
  // if (x >= NCOLS) return;
  // if (y >= NROWS) return;
  pos = (y * NCOLS_2) + (x>>1);
  if (x%2) {
    screen[pos] &= 0x0f;
    screen[pos] |= (a << 4);
  } else {
    screen[pos] &= 0xf0;
    screen[pos] |= (a & 0x0f);
  }
}

// draw a single character glyph with fixed spacing
U8 screen_char(U16 col, U16 row, char gl, U8 a) {
  static U8 x, y;
  for(y=0; y<FONT_CHARH; y++) {
    for(x=0; x<FONT_CHARW; x++) {
      if( (font_data[gl - FONT_ASCII_OFFSET].data[x] & (1 << y))) {
	screen_pixel(x+col, y+row, a);
      } else {
	screen_pixel(x+col, y+row, 0);
      }
    }
  }
  return x+1;
}


// draw a single character glyph with proportional spacing
U8 screen_char_squeeze(U16 col, U16 row, char gl, U8 a) {
  static U8 y, x;
  static U8 xnum;
  static const glyph_t * g;
  g = &(font_data[gl - FONT_ASCII_OFFSET]);
  xnum = FONT_CHARW - g->first - g->last;
  
  for(y=0; y<FONT_CHARH; y++) {
    for(x=0; x<xnum; x++) {
      if( (g->data[x + g->first] & (1 << y))) {
	screen_pixel(x + col, y + row, a);
      } else {
	screen_pixel(x + col, y + row, 0);
      }
    }
  }
  return xnum;
}

// draw a string with fixed spacing
U8 screen_string_fixed(U16 x, U16 y, char *str, U8 a) {
  while(*str != 0) {
    x += screen_char(x, y, *str, a) + 1;
    str++;
  }
  return x;
}

/*
// draw a string with proportional spacing
U8 screen_string_squeeze(U16 x, U16 y, char *str, U8 a) {
  while(*str != 0) {
    x += screen_char_squeeze(x, y, *str, a);
    zero_col(x, y);
    // extra pixel... TODO: maybe variable spacing here
    x++;
    str++;
  }
  return x;
}
*/

// draw a string (default) 
inline U8 screen_string(U16 x, U16 y, char *str, U8 a) {
  return screen_string_squeeze(x, y, str, a);
}
////////////////////////


// print some characters of text
u8 screen_string_squeeze(u16 x, u16 y, char* str, u8 hl) {
  

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

void screen_refresh(void) {
}
