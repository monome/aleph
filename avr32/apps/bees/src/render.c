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

//--------------------------
//---- extern vars
region* headRegion = NULL;
region* footRegion[4] = { NULL, NULL, NULL, NULL };
region* tmpRegion = NULL;

//------------------------
//---- static vars

static region headRegion_pr = 	{ .w=128, .h=8, .x = 0, .y = 0  };
static region footRegion_pr[4] = {
  { .w=32, .h=8, .x = 0,  .y = 56 },
  { .w=32, .h=8, .x = 32, .y = 56 },
  { .w=32, .h=8, .x = 64, .y = 56 },
  { .w=32, .h=8, .x = 96, .y = 56 },
};

//static region selectRegion_pr = { .w=128, .h=8, .x = 0, .y = 24 };
static region tmpRegion_pr = 	{ .w=128, .h=8, .x = 0, .y = 0  };

// dummy region with full-screen dimensions but no data
static region dummyRegion = { .w=128, .h=64, .x=0, .y=0, .data=NULL };

// pointer to current page scroll region
static region* scrollRegion = NULL;

// scroller
static scroll centerScroll;

// static line buffer
char lineBuf[LINEBUF_LEN];
static char *pline = lineBuf;
static char * const pLineEnd = lineBuf + LINEBUF_LEN - 1;

static const u8 colorHiFront = 	0xf;
static const u8 colorHiBack = 	0x2;
static const u8 colorLoFront = 	0xa; 
static const u8 colorLoBack = 	0x0;

// byte offsets for text lines
static u32 scrollLines[8] = {
  0	,
  1024	,
  2048	,
  3072	,
  4096	,
  5120	,
  6144	,
  7168
};

// check dirty flag and update region

//// test:
//static inline void region_update(region* r) {
void region_update(region* r) {
  if(r->dirty) {
    screen_draw_region(r->x, r->y, r->w, r->h, r->data);
    r->dirty = 0;
  }
}

//-----------------------
//---- extern functions

// initialize
void render_init(void) {
  /// dbg
  /* static region** allRegions[4] = { */
  /*   &headRegion, */
  /*   &footRegion, */
  /*   //    &selectRegion, */
  /*   &tmpRegion, */
  /* }; */

  /* u8 i; */
  ////////

  print_dbg("\r\n init rendering regions.. ");
  // screen regions allocated here
  region_alloc((region*)(&headRegion_pr));
  region_alloc((region*)(&(footRegion_pr[0])));
  region_alloc((region*)(&(footRegion_pr[1])));
  region_alloc((region*)(&(footRegion_pr[2])));
  region_alloc((region*)(&(footRegion_pr[3])));
  //  region_alloc((region*)(&selectRegion_pr));
  region_alloc((region*)(&tmpRegion_pr));

  headRegion = &headRegion_pr;
  footRegion[0] = &(footRegion_pr[0]);
  footRegion[1] = &(footRegion_pr[1]);
  footRegion[2] = &(footRegion_pr[2]);
  footRegion[3] = &(footRegion_pr[3]);
  //  selectRegion = &selectRegion_pr;
  tmpRegion = &tmpRegion_pr;

  // scroll init needs to compute offsets based on region size
  // use a dummy region with correct dimensions but no data
  scrollRegion = &dummyRegion;
  scroll_init(&centerScroll, scrollRegion);
}

// update
void render_update(void) {
  app_pause();

  // scrolling region
  if((centerScroll.reg)->dirty) {
    scroll_draw(&centerScroll);
  }
  // standard regions

  region_update(headRegion);
  region_update(footRegion[0]);
  region_update(footRegion[1]);
  region_update(footRegion[2]);
  region_update(footRegion[3]);
  app_resume();
}

// set current header region
void render_set_head_region(region* reg) {
  headRegion = reg;
  headRegion->dirty = 1;
}

// set current footer region
void render_set_foot_region(region* reg[4]) {
  footRegion[0] = reg[0];
  footRegion[1] = reg[1];
  footRegion[2] = reg[2];
  footRegion[3] = reg[3];
  footRegion[0]->dirty = 1;
  footRegion[1]->dirty = 1;
  footRegion[2]->dirty = 1;
  footRegion[3]->dirty = 1;
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
#warning TODO: fix string-editing render
      //     x += screen_char_fixed_back(x, y, str[i], 0x0, 0xa);
      ++x;
    } else {
      // #warning TODO: fix string-editing render
      //      x += screen_char_squeeze_back(x, y, str[i], 0x7, 0x0);
      ++x;
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
    *pline = *str;
    ++pline;
    ++str;
  }
}

// write int to top of line buffer
 inline void println_int(int val, int pos) {
  pline = lineBuf + pos;
  appendln_idx_lj(val);
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
/// very fast, for short unsigned values!
inline void appendln_idx_lj(u16 val) {
  u16 dig = 0;
  u16 rem = 0;
  // 3 digits only 
  if(val > 999) { rem = 999; } else { rem = val; }

  dig = val / 100U;
  rem -= dig * 100U;
  *pline = '0' + dig;
  ++pline;

  dig = rem / 10U;
  rem -= dig * 10U;
  *pline = '0' + dig;
  ++pline;

  *pline = '0' + rem;
  ++pline;    
}

// append char to line buffer
 inline void appendln_char(char c) {  
  *pline++ = c;
}

// clear line buffer
inline void clearln(void) {
  pline = lineBuf;
  memset(pline, ' ', LINEBUF_LEN);
}

 inline void endln(void) {
  *(pline) = '\0';
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
    *pdst = *psrc;
    ++psrc;
    ++pdst;
  } 
  selectRegion->dirty = 1;
}

// copy temp data to center of scroll region (clipping)
/// FIXME: abstract this and place in region.c
void render_to_scroll_center(void) {
  u8* psrc;
  u8* pdst;
  u32 i;
  psrc = tmpRegion->data;
  pdst = centerScroll.reg->data + centerScroll.byteOff + SCROLL_CENTER_OFFSET;
  for(i=SCROLL_BYTES_PER_LINE; i>0; --i) {
    *pdst = *psrc;
    ++psrc;
    ++pdst;
  } 
  scrollRegion->dirty = 1;
}

// copy from center of scroll region to select (adding highlight)
void render_from_scroll_center(void) {
  u8* psrc;
  u8* pdst;
  u32 i;
  psrc = centerScroll.reg->data + centerScroll.byteOff + SCROLL_CENTER_OFFSET;
  pdst = selectRegion->data;
  for(i=SCROLL_BYTES_PER_LINE; i>0; --i) {
    *pdst = *psrc;
    ++psrc;
    ++pdst;
  } 
  selectRegion->dirty = 1;  
}


// add data to top of scroll region
void render_to_scroll_top(void) {
  scroll_region_back(&centerScroll, tmpRegion);
}


// add data to bottom of scroll region (clipping)
void render_to_scroll_bottom(void) {
  scroll_region_front(&centerScroll, tmpRegion);
}

//+++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++


/* note: this type of bounds check (on dstOff in functions below)
   is only intendeded to catch line offsets.
   in other words, it is assumed
   that scroll offset is always advanced by multiples of line length.
  */

// render tmp region to arbitrary line of scroll region, with highlight flag
// no update is performed
void render_to_scroll_line(u8 n, u8 hl) {
  u8* src;
  u8* dst;
  u8* dstMax;
  // data offset in scroll
  s32 dstOff = centerScroll.byteOff + scrollLines[n];
  while(dstOff >= scrollRegion->len) { dstOff -= scrollRegion->len; }
  dst = scrollRegion->data + dstOff;
  // setup copy
  src = tmpRegion->data;
  dstMax = dst + tmpRegion->len - 1;
  // copy and apply color map based on HL
  // it is assumed that tmp buffer uses colors [0, >0]
  if(hl) {
    while(dst < dstMax) {
      if(*src > 0) {
	*dst = colorHiFront;
      } else {
	*dst = colorHiBack;
      }
      src++;
      dst++;
    }
  } else {
    while(dst < dstMax) {
      if(*src > 0) {
	*dst = colorLoFront;
      } else {
	*dst = colorLoBack;
      }
      src++;
      dst++;
    }
  }
  scrollRegion->dirty = 1;
}

/// apply highlight  given line of scroll
void render_scroll_apply_hl(u8 n, u8 hl) {
  u8* dst;
  u8* dstMax;
  // data offset in scroll
  s32 dstOff = centerScroll.byteOff + scrollLines[n];
  while(dstOff >= scrollRegion->len) { dstOff -= scrollRegion->len; }
  // setup bounds
  dst = scrollRegion->data + dstOff;
  dstMax = dst + tmpRegion->len - 1;

  if(hl) {
    while(dst < dstMax) {
      if(*dst < colorLoFront) {
	*dst = colorHiBack;
      } else {
	*dst = colorHiFront;
      }
      dst++;
    }  
  } else {
    while(dst < dstMax) {
      if(*dst < colorHiFront) {
	*dst = colorLoBack;
      } else {
	*dst = colorLoFront;
      }
      dst++;
    }  
  }
  scrollRegion->dirty = 1;
}

// fill scroll line with color
//extern void render_fill_scroll_line(u8 n, u8 col) {
  /* u8* dst; */
  /* u8* dstMax; */
  /* // data offset in scroll */
  /* s32 dstOff = centerScroll.byteOff + scrollLines[n]; */
  /* while(dstOff > scrollRegion->len) { dstOff -= scrollRegion->len; } */
  /* // setup bounds */
  /* dst = scrollRegion->data + dstOff; */
  /* dstMax = dst + tmpRegion->len - 1; */
  //  while(dst < dstMax)
//}
