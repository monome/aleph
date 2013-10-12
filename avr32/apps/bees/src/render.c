/* render.c
   bees
   aleph
   
   render functions common to pages.

 */


// std
#include <string.h>

#include "print_funcs.h"

// common
#include "fix.h"
#include "simple_string.h"
#include "types.h"

// avr32
#include "app.h"
#include "font.h"
#include "region.h"
#include "screen.h"

// bees
#include "render.h"

#define LINEBUF_LEN 64

//--------------------------
//---- extern vars
region* headRegion = NULL;
region* footRegion = NULL;
region* selectRegion = NULL;
region* tmpRegion = NULL;

//------------------------
//---- static vars

// pointer to current page scroll region
static region* scrollRegion = NULL;

// scroller
static scroll centerScroll;

// static line buffer
static char lineBuf[LINEBUF_LEN];
//static char numBuf[FIX_DIG_TOTAL] = "           ";
static char *pline = lineBuf;
static char * const pLineEnd = lineBuf + LINEBUF_LEN - 1;

// check dirty flag and update region
static inline void region_update(region* r) {
  if(r->dirty) {
    screen_draw_region(r->x, r->y, r->w, r->h, r->data);
    r->dirty = 0;
  }
}

//-----------------------
//---- extern functions

// initialize
void render_init(void) {
  // screen regions allocated here
  region_alloc((region*)(&headRegion));
  region_alloc((region*)(&footRegion));
  region_alloc((region*)(&selectRegion));
  region_alloc((region*)(&tmpRegion));
  // init each page for rendering
  // doing this in menu.c
  /* init_page_ins(); */
  /* init_page_outs(); */
  /* init_page_presets(); */
  /* init_page_ops(); */
  /* init_page_scenes(); */
  /* init_page_dsp(); */
  /* init_page_gathered(); */
  /* init_page_play(); */
}

// update
void render_update(void) {
  app_pause();
  // scrolling region
  if(centerScroll.reg->dirty) {
    scroll_draw(&centerScroll);
  }
  // standard regions
  region_update(headRegion);
  region_update(footRegion);
  region_update(selectRegion);
  app_resume();
}

// set current header region
void render_set_head_region(region* reg) {
  headRegion = reg;
  headRegion->dirty = 1;
}

// set current footer region
void render_set_foot_region(region* reg) {
  footRegion = reg;
  footRegion->dirty = 1;
}

// set current scroll region
void render_set_scroll_region(region* reg) {
  scrollRegion = reg;
  scrollRegion->dirty = 1;
  centerScroll.reg = reg;
}


// draw editing string at given position, with cursor highlight
void draw_edit_string(u8 x, u8 y, char* str, u8 len) {
  //// FIXME... need more abstraction
  u8 i;
  y *= FONT_CHARH;
  for(i=0; i<len; i++) {
    if(str[i] == 0) { return; }
    if(i == curPage->cursor) {
      //     x += screen_char_fixed_back(x, y, str[i], 0x0, 0xa);
      x++;
    } else {
      //      x += screen_char_squeeze_back(x, y, str[i], 0x7, 0x0);
      x++;
    }
  }
}

/// stupid string lib replacements.. 
// all act on static string buffer in render.c

// write to top of line buffer
 inline void println(const char* str, int pos) {
  pline = lineBuf + pos;
  appendln(str);
}

// append to line buffer
 inline void appendln(const char* str) {
  //  print_dbg("\n\r line buffer start: ");
  //  print_dbg_hex(pline);
  while((*str != 0) && (pline <= pLineEnd)) {
    *pline++ = *str++;
  }
}

// write int to top of line buffer
 inline void println_int(int val, int pos) {
  pline = lineBuf + pos;
  appendln_int_lj(val);
}

// append int to line buffer
 inline void appendln_int_rj(int val, int len) {
  // bounds check :
  //  int rem = (int)pLineEnd -  (int)pline;
  //  if (len > rem) len = rem;
  itoa_whole(val, pline, len); 
  pline += len; 
}

// append int to line buffer (left justified, no bounds)
 inline void appendln_int_lj(int val) {
  pline += itoa_whole_lj(val, pline); 
}

// append char to line buffer
 inline void appendln_char(char c) {  
  *pline++ = c;
}

// clear line buffer
 inline void clearln(void) {
  memset(pline, ' ', LINEBUF_LEN);
}

 inline void endln(void) {
  *(pline) = 0;
}

// get the line buffer
 char* get_line_buf(void) {
   return lineBuf;
}

// get current y-offset for center line in scroll
 u8 get_yoff(void) {
   u8 ret = centerScroll.yOff + SCROLL_CENTER_Y_OFFSET;
   if(ret > centerScroll.reg->h) {
     ret -= centerScroll.reg->h;
   }
   return ret;
}


// copy temp data to selection (adding highlight)
void render_to_select(void) {
  u8* psrc;
  u8* pdst;
  u32 i;
  psrc = tmpRegion->data;
  pdst = selectRegion->data;
  for(i=SCROLL_BYTES_PER_LINE; i>0; --i) {
    *pdst = *psrc | COLOR_HL;
    ++psrc;
    ++pdst;
  } 
  selectRegion->dirty = 1;
}

// copy temp data to center of scroll region (clipping)
void render_to_scroll_center(void) {
  u8* psrc;
  u8* pdst;
  u32 i;
  psrc = tmpRegion->data;
  pdst = centerScroll.reg->data + centerScroll.byteOff + SCROLL_CENTER_BYTE_OFFSET;
  for(i=SCROLL_BYTES_PER_LINE; i>0; --i) {
    *pdst = *psrc & COLOR_UNSELECT;
    ++psrc;
    ++pdst;
  } 
  centerScroll.reg->dirty = 1;
}

// add data to top of scroll region (clipping)
void render_to_scroll_top(void) {
  u8* psrc;
  u8* pdst;
  u32 i;
  psrc = tmpRegion->data;
  for(i=SCROLL_BYTES_PER_LINE; i>0; --i) {
    *pdst = *psrc & COLOR_UNSELECT;
    ++psrc;
    ++pdst;
  } 
  centerScroll.reg->dirty = 1;
}

