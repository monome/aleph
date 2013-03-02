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

#if ARCH_LINUX
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
#endif

//-----------------------------

//-----------------------------
//---- variables
const U8 lines[CHAR_ROWS] = { 0, 8, 16, 24, 32, 40, 48, 56 };

// pixel buffer
static U8 screen[GRAM_BYTES];

//static U8 text[CHAR_ROWS][CHAR_COLS];
//static char linebuf[CHAR_COLS]; // line bu
#if ARCH_LINUX 
// use characters to represent shades of grey
#define NUM_SHADES 16
static const char shades[NUM_SHADES] = {
  ' ', '.', ':', ';', '!', '(', '[', '|', 'O', '0' , 'S', '@', 'H', 'W', '%', '#'
};

#endif

//------ static funcs

// fill one column in a line of text with blank pixels (for spacing)
static void zero_col(U16 x, U16 y);
static void zero_col(U16 x, U16 y) {
  static U8 i;
  for(i=0; i<FONT_CHARH; i++) {
    screen_pixel(x, y+i, 0);
  }
}

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
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, COLOR_BLUE);
    init_pair(4, COLOR_BLUE, COLOR_WHITE);
    init_pair(5, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(6, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(7, COLOR_RED, COLOR_MAGENTA);
    init_pair(8, COLOR_MAGENTA, COLOR_RED);
    init_pair(9, COLOR_YELLOW, COLOR_RED);
    init_pair(10, COLOR_RED, COLOR_YELLOW);
    init_pair(11, COLOR_CYAN, COLOR_YELLOW);
    init_pair(12, COLOR_YELLOW, COLOR_CYAN);
    init_pair(13, COLOR_YELLOW, COLOR_CYAN);
    init_pair(14, COLOR_CYAN, COLOR_WHITE);
    init_pair(15, COLOR_WHITE, COLOR_RED);
    init_pair(16, COLOR_CYAN, COLOR_WHITE);
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
    menu_handleKey(eKeyMode, 1);
    break;
    /// "encoders"
  case KEY_ENC_A_UP:
    menu_handleKey(eKeyEncUpA, 1);
    break;
  case KEY_ENC_A_DOWN:
    menu_handleKey(eKeyEncDownA, 1);
    break;
  case KEY_ENC_B_UP:
    menu_handleKey(eKeyEncUpB, 1);
    break;
  case KEY_ENC_B_DOWN:
    menu_handleKey(eKeyEncDownB, 1);
    break;
  case KEY_ENC_C_UP:
    menu_handleKey(eKeyEncUpC, 1);
    break;
  case KEY_ENC_C_DOWN:
    menu_handleKey(eKeyEncDownC, 1);
    break;
  case KEY_ENC_D_UP:
    menu_handleKey(eKeyEncUpD, 1);
    break;
  case KEY_ENC_D_DOWN:
    menu_handleKey(eKeyEncDownD, 1);
    break;
    // function keys
  case KEY_FN_A_DOWN:
    menu_handleKey(eKeyFnDownA, 1);
    break;
  case KEY_FN_A_UP:
    menu_handleKey(eKeyFnUpA, 1);
    break;
  case KEY_FN_B_DOWN:
    menu_handleKey(eKeyFnDownB, 1);
    break;
  case KEY_FN_B_UP:
    menu_handleKey(eKeyFnUpB, 1);
    break;
  case KEY_FN_C_DOWN:
    menu_handleKey(eKeyFnDownC, 1);
    break;
  case KEY_FN_C_UP:
    menu_handleKey(eKeyFnUpC, 1);
    break;
  case KEY_FN_D_DOWN:
    menu_handleKey(eKeyFnDownD, 1);
    break;
  case KEY_FN_D_UP:
    menu_handleKey(eKeyFnUpD, 1);
    break;
  default:
    break;
  }
  return run;
}


///////////////////
////////////////
// from avr32


//----- drawing routine
// FIXME: abstract drawing from hardware,
// move and rename (e.g. draw_*), 

// draw a single pixel
void screen_pixel(U16 x, U16 y, U8 a) {
  static U32 pos;
  // if (x >= NCOLS) return;
  // if (y >= NROWS) return;
  pos = (y * NCOLS__2) + (x>>1);
  if (x%2) {
    screen[pos] &= 0x0f;
    screen[pos] |= (a << 4);
  } else {
    screen[pos] &= 0xf0;
    screen[pos] |= (a & 0x0f);
  }
}

// get value of pixel
U8 screen_get_pixel(U8 x, U8 y) {
  static U32 pos;
  // if (x >= NCOLS) return;
  // if (y >= NROWS) return;
  pos = (y * NCOLS__2) + (x>>1);
  if (x%2) {
    return (screen[pos] & 0xf0) >> 4; 
   } else {
    return screen[pos] & 0x0f;
  }
}

// draw a single character glyph with fixed spacing
U8 screen_char_fixed(U16 col, U16 row, char gl, U8 a) {
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
    x += screen_char_fixed(x, y, *str, a) + 1;
    str++;
  }
  return x;
}

// draw a string with proportional spacing
U8 screen_string_squeeze(U16 x, U16 y, char *str, U8 a) {
  while(*str != 0) {
    x += screen_char_squeeze(x, y, *str, a);
    zero_col(x, y);
    // extra pixel... TODO: maybe variable spacing here
    x++;
    str++;
  }
  
  //  refresh = 1;

#ifdef ARCH_AVR32
  refresh = 1;
#else 
  screen_refresh();
#endif

  return x;
}

// draw a string (default) m
inline U8 screen_string(U16 x, U16 y, char *str, U8 a) {
  return screen_string_squeeze(x, y, str, a);
}

// print a formatted integer
U8 screen_int(U16 x, U16 y, S16 i, U8 a) {
  //  static char buf[32];
  //  snprintf(buf, 32, "%d", (int)i);
    static char buf[FIX_DIG_TOTAL];
  //snprintf(buf, 32, "%.1f", (float)f);
    //  print_fix16(buf, (u32)i << 16 );
    itoa_whole(i, buf, 5);
  //buf = ultoa(int);
  return screen_string_squeeze(x, y, buf, a);
}

// print a formatted float
/*
U8 screen_float(U16 x, U16 y, F32 f, U8 a) {
  static char buf[32];
  snprintf(buf, 32, "%.1f", (float)f);
  return screen_string_squeeze(x, y, buf, a);
}
*/
// print a formatted fix_t
U8 screen_fix(U16 x, U16 y, fix16_t v, U8 a) {
  static char buf[FIX_DIG_TOTAL];
  //snprintf(buf, 32, "%.1f", (float)f);
  print_fix16(buf, v);
  return screen_string_squeeze(x, y, buf, a);
}

// send screen buffer contents to OLED
void screen_refresh(void) {
  U16 i;
  u16 x, y;
  u8 pix, j;
  char buf[2];
  u8 color;
  buf[1] = 0;
  x = 0;
  y = 0;
  for(i=0; i<(GRAM_BYTES); i++) { 
    for(j=0; j<2; j++) {
      if(j==0) {
	pix = screen[i] & 0xf;
      } else {
	pix = screen[i] >> 4;
      }
      color = pix + 1;
      attron(COLOR_PAIR(color));
      buf[0] = shades[pix];
      mvprintw(y, x, buf);
      attroff(COLOR_PAIR(color));
      x++;
      if (x > NCOLS_1) {
	x = 0;
	y++;
      }
    }
    //write_data(screen[i]);  
    //write_data(i % 0xf);
  }
  refresh();
  //  cpu_irq_enable();
  //  Enable_global_interrupt();
}

// fill a line with blank space to end
void screen_blank_line(U16 x, U16 l) {
  U8 i, j, y;
  y = lines[l];
  for(i=x; i<NCOLS; i++) {
    for(j=y; j<(FONT_CHARH + y); j++) {
      screen_pixel(i, j, 0);
    }
  }
 }

// highlight a line
void screen_hl_line(U16 x, U16 y, U8 a) {
  U8 i, j;
  for(i=x; i<NCOLS; i++) {
    for(j=y; j<(y+FONT_CHARH); j++) {
      if (screen_get_pixel(i, j) == 0) {
	screen_pixel(i, j, a);
      }
    }
  }
#ifdef ARCH_AVR32
  refresh = 1;
#else 
  screen_refresh();
#endif
}

// draw a line and blank to end
U8 screen_line(U16 x, U16 l, char *str, U8 hl) {
  u8 y;
  // FIXME
  //  hl = ( (hl << 1) & 0xf);
  //  if (hl ) { hl =0xf;a }

  y = lines[l];

  x = screen_string(x, y, str, hl);
  screen_blank_line(x, l);

  //  print_dbg("\r\n");
  //  if(hl > 2) { print_dbg("__"); }
  //  print_dbg(str);
#ifdef ARCH_AVR32
  refresh = 1;
#else 
  screen_refresh();
#endif
  return NCOLS;
}

// fill graphics ram with a test pattern
void screen_test_fill(void) {
  u32 i;
  u32 x=0;
  u32 y=0;
  for(i=0; i<font_nglyphs; i++) {

    x = x + screen_char_squeeze(x, y, i + FONT_ASCII_OFFSET, 0xf);
    x++;
    if (x > NCOLS) {
      x -= NCOLS;
      y += FONT_CHARH;
    }
  }
  //  refresh = 1;
}
