/* redraw.c
 * bees
 * aleph
 *
 * page-specific redraw functinos.
 */

// undefine to use fixed-point output formatting
//#define SNPRINTF

// std
#include <string.h>
//#include <stdio.h>

#ifdef ARCH_AVR32
// #include "print_funcs.h"
#endif

// aleph
#include "files.h"
#include "fix.h"
#include "redraw.h"
#include "screen.h"
#include "types.h"

//------------------------
//----- static variables

// line buffer
static char lineBuf[CHAR_COLS];
//static char numBuf[FIX_DIG_TOTAL] = "           ";
static char *pline = lineBuf;
static char * const pLineEnd = lineBuf + CHAR_COLS_1;

//---------------------------------
//---- static functions

// write to top of line buffer
static inline void println(const char* str, int pos);
// append to line buffer
static inline void appendln(const char* str);
// write int to top of line buffer
static inline void println_int(int val, int pos);
// append int to line buffer (right justified, with bound argument)
static inline void appendln_int_rj(int val, int len);
// append int to line buffer (left justified, no bounds)
static inline void appendln_int_lj(int val);
// append char to line buffer
static inline void appendln_char(char c);
// clear line buffer
static inline void clearln(void);
// move to given position in line buffer
//static inline void mvln(u8 pos);
// end line buffer (write a zero to next position)
static inline void endln(void);

//// line redraws
static void draw_line_dsp(s32 n, u16 num, u8 y, u8 hl);
static void draw_line_fw(s32 n, u16 num, u8 y, u8 hl);


//==================================================
//==== redraw dsp page
void redraw_dsp(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  u16 num = files_get_dsp_count();
  /* // print_dbg("\r\n count of DSP files: "); */
  /* // print_dbg_ulong(num); */
  // print selection at center
  nCenter = curPage->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  draw_line_dsp(n, num, y, 15);
  screen_hl_line(0, y, 1);
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    draw_line_dsp(n, num, y, 4);
  }
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  // print higher entries
  while (y < CHAR_ROWS_2) {
    n++;
    y++;
    draw_line_dsp(n, num, y, 4);
  }
  screen_line(0, 0, "MODULES", 12);
  // draw footer (function labels)
  //  screen_line(0, CHAR_ROWS_1, "LOAD DEFAULT", 10);
  screen_line(0, CHAR_ROWS_1, "LOAD", 10);
}

//==================================================
//==== redraw firmware page
void redraw_fw(void) {
  u8 y = 0;                       // which line
  s32 n, nCenter;         // which list entry
  u16 num = files_get_firmware_count();
  /* // print_dbg("\r\n count of DSP files: "); */
  /* // print_dbg_ulong(num); */
  // print selection at center
  nCenter = curPage->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  draw_line_fw(n, num, y, 15);
  screen_hl_line(0, y, 1);
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    draw_line_fw(n, num, y, 4);
  }
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  // print higher entries
  while (y < CHAR_ROWS_2) {
    n++;
    y++;
    draw_line_fw(n, num, y, 4);
  }
  screen_line(0, 0, "APPLICATIONS", 12);
  // draw footer (function labels)
  screen_line(0, CHAR_ROWS_1, "WRITE", 10);
}

//------ line redraws


// draw line of dsp page
void draw_line_dsp(s32 n, u16 num, u8 y, u8 hl) {
  screen_blank_line(0, y);

  if (n < 0  || n >= num ) {
    return;
  } 

  println( (const char*)files_get_dsp_name(n), 0);
  endln(); screen_string(0, y, lineBuf, hl); 
}

// draw line of firmware page
void draw_line_fw(s32 n, u16 num, u8 y, u8 hl) {
  screen_blank_line(0, y);

  if (n < 0  || n >= num ) {
    return;
  } 

  println( (const char*)files_get_firmware_name(n), 0);
  endln(); screen_string(0, y, lineBuf, hl); 
}

//--------- utils

///// snprintf replacement
// write to top of line buffer
static inline void println(const char* str, int pos) {
  pline = lineBuf + pos;
  appendln(str);
}

// append to line buffer
static inline void appendln(const char* str) {
  //  // print_dbg("\n\r line buffer start: ");
  //  // print_dbg_hex(pline);
  while((*str != 0) && (pline <= pLineEnd)) {
    *pline++ = *str++;
  }
}

// write int to top of line buffer
static inline void println_int(int val, int pos) {
  pline = lineBuf + pos;
  appendln_int_lj(val);
}

// append int to line buffer
static inline void appendln_int_rj(int val, int len) {
  // bounds check :
  //  int rem = (int)pLineEnd -  (int)pline;
  //  if (len > rem) len = rem;
  itoa_whole(val, pline, len); 
  pline += len; 
}

// append int to line buffer (left justified, no bounds)
static inline void appendln_int_lj(int val) {
  pline += itoa_whole_lj(val, pline); 
}

// append char to line buffer
static inline void appendln_char(char c) {  
  *pline++ = c;
}

// clear line buffer
static inline void clearln(void) {
  memset(pline, ' ', CHAR_COLS);
}

static inline void endln(void) {
  *(pline) = 0;
}
