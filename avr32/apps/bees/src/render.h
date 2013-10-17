#ifndef _RENDER_H_
#define _RENDER_H_

// avr32
#include "font.h"
#include "region.h"
// bees
#include "menu.h"
#include "menu_protected.h"

//========================================
//=== positioning constants

// all scroll regions on the pages have the same dimensions.
// selection is always at the center.

// center line
#define SCROLL_CENTER_LINE 	3
// lines above center
#define SCROLL_LINES_BELOW 	3
#define SCROLL_LINES_ABOVE 	4
#define SCROLL_LINES_ABOVE_1 	(SCROLL_LINES_ABOVE - 1)
#define SCROLL_BYTES_PER_LINE (FONT_CHARH * 128)
// byte offset for top of center row
#define SCROLL_CENTER_OFFSET (SCROLL_CENTER_LINE * SCROLL_BYTES_PER_LINE)
// in pixels, y-offset for top of center row
#define SCROLL_CENTER_Y_OFFSET (SCROLL_CENTER_LINE * FONT_CHARH)
/// offset for last row
#define SCROLL_LAST_LINE_OFFSET (SCROLL_BYTES_PER_LINE * 7)

// x-offset in pixels for value field on each line
#define LINE_VAL_POS 64

// line buffer, length in characters
#define LINEBUF_LEN 32


//================================
//===== extern vars
char lineBuf[LINEBUF_LEN];

// screen regions
extern region* headRegion;
extern region* footRegion;
extern region* selectRegion;
extern region* tmpRegion;

//======================================
//=== extern functions

// initialize
extern void render_init(void);
// draw dirty regions to the screen
extern void render_update(void);
// set current header region
extern void render_set_head_region(region* reg);
// set current footer region
extern void render_set_foot_region(region* reg);
// set current scroll region
extern void render_set_scroll_region(region* reg) ;

// copy data to selection (adding highlight)
extern void render_to_select(void);
// copy from center of scroll region to select (adding highlight)
extern void render_from_scroll_center(void);
// copy temp data to center of scroll region (clipping)
extern void render_to_scroll_center(void);
// add data to top of scroll region (clipping)
extern void render_to_scroll_top(void);
// add data to bottom of scroll region (clipping)
extern void render_to_scroll_bottom(void);


// render tmp region to arbitrary line, with highlight level
extern void render_to_scroll_line(u8 n, u8 hl);
// fill scroll line with color
extern void render_fill_scroll_line(u8 n, u8 col);
// add/remove highlight on given line of scroll
extern void render_scroll_apply_hl(u8 n, u8 hl);

/////
/// test (can/should be declared static)
extern void region_update(region* r);
////


//----------------------------------------------
/// stupid string formatting... 
// all act on static string buffer in render.c

// draw to top of string buffer
extern void println( const char* str, int pos);
// append to line buffer
extern void appendln( const char* str);
// write int to top of line buffer
extern void println_int( int val, int pos);
// append int to line buffer (right justified, with bound argument)
extern void appendln_int_rj( int val, int len);
// append int to line buffer (left justified, no bounds)
/// for short unsigned values (3 digits)
extern void appendln_idx_lj(u16 val);
// append char to line buffer
extern void appendln_char( char c);
// clear line buffer
extern void clearln( void);
// end line buffer (write a zero to next position)
extern void endln( void);

// draw editing string at given position, with cursor highlight
extern void draw_edit_string( u8 x, u8 y, char* str, u8 len);

//-----------------------------
//----- extra miscellaneous getters
// get the line buffer (a string)
extern char* get_line_buf(void);
// get current y-offset in scroll
extern u8 get_yoff(void);

#endif // header guard
