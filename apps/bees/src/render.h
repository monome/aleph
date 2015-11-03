#ifndef _RENDER_H_
#define _RENDER_H_

// avr32
#include "font.h"
#include "region.h"
// bees
#include "pages.h"

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
// underline at bottom of line region
#define LINE_UNDERLINE_LEN 127
#define LINE_UNDERLINE_OFFSET (SCROLL_BYTES_PER_LINE - LINE_UNDERLINE_LEN - 1)

// x-offset in pixels for value field on each line
#define LINE_VAL_POS_SHORT 98
#define LINE_VAL_POS_LONG 78

// line buffer, length in characters
#define LINEBUF_LEN 32


//================================
//===== extern vars
char lineBuf[LINEBUF_LEN];

// screen regions
extern region* headRegion;
extern region* footRegion[4];
extern region* selectRegion;
extern region* lineRegion;

//======================================
//=== extern functions

// initialize
extern void render_init(void);

// render text to scrolling buffer during boot procedure
extern void render_boot(const char* str);

// draw dirty regions to the screen
extern void render_update(void);
// set current header region
extern void render_set_head_region(region* reg);
// set current footer region
extern void render_set_foot_region(region* reg[4]);
// set current scroll region
extern void render_set_scroll(scroll* scr) ;

// copy data to selection (adding highlight)
extern void render_to_select(void);
// copy from center of scroll region to select (adding highlight)
extern void render_from_scroll_center(void);
// copy temp data to center of scroll region (clipping)
extern void render_to_scroll_center(void);
// add line region to top of scroll region (clipping)
extern void render_to_scroll_top(void);
// add line region to bottom of scroll region (clipping)
extern void render_to_scroll_bottom(void);

// render tmp region to arbitrary line, with highlight level
extern void render_to_scroll_line(u8 n, u8 hl);
// fill scroll line with color
extern void render_fill_scroll_line(u8 n, u8 col);
// add/remove highlight on given line of scroll
extern void render_scroll_apply_hl(u8 n, u8 hl);

/////
/// this can and should be a static function
/// it is external right now for testing
/// update a single region
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
// clear line buffer, and reset position
extern void clearln( void);
// end line buffer (write a zero to next position)
extern void endln( void);

// draw editing string to given region, with cursor highlight
extern void render_edit_string( region* reg, char* str, u8 len, u8 cursor);

// draw preset name in header
extern void draw_preset_name(void);

// draw to the head region
extern void notify(const char* str);


#endif // header guard
