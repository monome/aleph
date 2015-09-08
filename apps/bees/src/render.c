/* render.c
   bees
   aleph
   
   render functions common to all the menu pages.

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
#include "preset.h"
#include "region.h"
#include "screen.h"

// bees
#include "render.h"

//---- extern vars
region* headRegion = NULL;
region* footRegion[4] = { NULL, NULL, NULL, NULL };
region* lineRegion = NULL;
region* selectRegion = NULL;

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
static region lineRegion_pr = 	{ .w=128, .h=8, .x = 0, .y = 0  };

// dummy region with full-screen dimensions but no data
// static region dummyRegion = { .w=128, .h=64, .x=0, .y=0, .data=NULL };

// pointer to current page scroll region
static region* pageScrollRegion = NULL;
// scroller
static scroll* pageCenterScroll = NULL;


// scrolling boot region
static region bootScrollRegion = {
  .w = 128, .h = 64, .x = 0, .y = 0
};
// scroller for boot region
static scroll bootScroll;

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

// static inline void region_update(region* r) {
//// test: extern
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
 
  // scrolling boot region
  region_alloc((region*)(&bootScrollRegion));
  scroll_init(&bootScroll, &bootScrollRegion);

  print_dbg("\r\n init rendering regions.. ");
  // screen regions allocated here
  region_alloc((region*)(&headRegion_pr));
  region_alloc((region*)(&(footRegion_pr[0])));
  region_alloc((region*)(&(footRegion_pr[1])));
  region_alloc((region*)(&(footRegion_pr[2])));
  region_alloc((region*)(&(footRegion_pr[3])));
  //  region_alloc((region*)(&selectRegion_pr));
  region_alloc((region*)(&lineRegion_pr));

  headRegion = &headRegion_pr;
  footRegion[0] = &(footRegion_pr[0]);
  footRegion[1] = &(footRegion_pr[1]);
  footRegion[2] = &(footRegion_pr[2]);
  footRegion[3] = &(footRegion_pr[3]);
  //  selectRegion = &selectRegion_pr;
  lineRegion = &lineRegion_pr;
}

// render text to scrolling buffer during boot procedure
extern void render_boot(const char* str) {
  int i;
  u8* p = bootScroll.reg->data;
  // first kind of dim the old stuff in the scroll region
  for(i=0; i<bootScroll.reg->len; i++) {
    if(*p > 0x4) { *p = 0x4; }
    p++;
  }
  scroll_string_front(&bootScroll, (char*)str);
  region_update(bootScroll.reg);
}


// update
void render_update(void) {
  app_pause();

  // scrolling region
  if((pageCenterScroll->reg)->dirty) {
    scroll_draw(pageCenterScroll);
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
void render_set_scroll(scroll* scr) {
  pageCenterScroll = scr;
  pageScrollRegion = scr->reg;
  pageScrollRegion->dirty = 1;
}

//----- stupid string lib replacements.. 
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
#if 0
  pline = atoi_idx(pline, val);
#else
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
#endif
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
/*
u8 get_yoff(void) {
   u8 ret = pageCenterScroll->yOff + SCROLL_CENTER_Y_OFFSET;
   if(ret > pageCenterScroll->reg->h) {
     ret -= pageCenterScroll->reg->h;
   }
   return ret;
}
*/


// copy temp data to selection (adding highlight)
void render_to_select(void) {
  u8* psrc;
  u8* pdst;
  u32 i;
  psrc = lineRegion->data;
  pdst = selectRegion->data;
  for(i=SCROLL_BYTES_PER_LINE; i>0; --i) {
    *pdst = *psrc;
    ++psrc;
    ++pdst;
  } 
  selectRegion->dirty = 1;
}

// copy temp data to center of scroll region (clipping)
/// FIXME: abstract this and place in region.c ?
void render_to_scroll_center(void) {
  u8* psrc;
  u8* pdst;
  u32 i;
  psrc = lineRegion->data;
  pdst = pageCenterScroll->reg->data + pageCenterScroll->byteOff + SCROLL_CENTER_OFFSET;
  for(i=SCROLL_BYTES_PER_LINE; i>0; --i) {
    *pdst = *psrc;
    ++psrc;
    ++pdst;
  } 
  pageScrollRegion->dirty = 1;
}

// copy from center of scroll region to select (adding highlight)
void render_from_scroll_center(void) {
  u8* psrc;
  u8* pdst;
  u32 i;
  psrc = pageCenterScroll->reg->data + pageCenterScroll->byteOff + SCROLL_CENTER_OFFSET;
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
  scroll_region_back(pageCenterScroll, lineRegion);
}


// add data to bottom of scroll region (clipping)
void render_to_scroll_bottom(void) {
  scroll_region_front(pageCenterScroll, lineRegion);
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
  s32 dstOff = pageCenterScroll->byteOff + scrollLines[n];
  while(dstOff >= pageScrollRegion->len) { dstOff -= pageScrollRegion->len; }
  dst = pageScrollRegion->data + dstOff;
  // setup copy
  src = lineRegion->data;
  dstMax = dst + lineRegion->len;
  // copy and apply color map based on HL
  if(hl) {
    while(dst < dstMax) {
      if(*src == 0) {
	*dst = colorHiBack;
      } else {
	*dst = *src;
      }
      ++src;
      ++dst;
    }
  } else {
    while(dst < dstMax) {
      if(*src > 0) {
	*dst = colorLoFront;
      } else {
	*dst = colorLoBack;
      }
      ++src;
      ++dst;
    }
  }
  pageScrollRegion->dirty = 1;
}

/// apply highlight  given line of scroll
void render_scroll_apply_hl(u8 n, u8 hl) {
  u8* dst;
  u8* dstMax;
  // data offset in scroll
  s32 dstOff = pageCenterScroll->byteOff + scrollLines[n];
  while(dstOff >= pageScrollRegion->len) { dstOff -= pageScrollRegion->len; }
  // setup bounds
  dst = pageScrollRegion->data + dstOff;
  dstMax = dst + lineRegion->len;

  if(hl) {
    while(dst < dstMax) {
      
      if(*dst >= colorLoFront) {
	*dst = colorHiFront;
      } else {
	if(*dst == 0) {
	  *dst = colorHiBack;
	}
      }
      dst++;
    }  
  } else {
    while(dst < dstMax) {
      if(*dst <= colorHiBack) {
	*dst = colorLoBack;
      } else {
	if(*dst == colorHiFront) {
	  *dst = colorLoFront;
	}
      }
      dst++;
    }  
  }
  pageScrollRegion->dirty = 1;
}


// draw editing string to given region, with cursor highlight
void render_edit_string(region* reg, char* str, u8 len, u8 cursor) {
  u8 i;
  u8* dst = (u8*)reg->data;
  u32 off = 0;
  //  const u32 squarePx = (FONT_CHARW+2) * FONT_CHARH;
  u32 dif;
  region_fill(reg, 0x0);
  bool wasNull = 0;
  for(i=0; i<len; ++i) {
    if(i == cursor) {
      // hack a column fill with a space character
      font_glyph(' ', dst, reg->w, 0x0, 0xf);
      dst += 2; off += 2;

      if(str[i] == '\0') {
	wasNull = 1; 
      }  else {
	if(wasNull) {
	  font_glyph_fixed(str[i], dst, reg->w, 0x0, 0x1);
	} else {
	  font_glyph_fixed(str[i], dst, reg->w, 0x0, 0xf);
	}
      }
      dif = FONT_CHARW + 2;
      dst += dif;
      off += dif;
    } else {
      // not the cursor
      if(str[i] == '\0') {
	dif = font_glyph(' ', dst, reg->w, 0x1, 0x0) + 1;
	wasNull = 1; 
      } else {
	if(wasNull) {
	  dif = font_glyph(str[i], dst, reg->w, 0x1, 0x0) + 1;
	} else {
	dif = font_glyph(str[i], dst, reg->w, 0xf, 0x0) + 1;
	}
    }
      dst += dif;
      off += dif;
    }
  }
  reg->dirty = 1;
  print_dbg("\r\n edited string: ");
  print_dbg(str);
}


// draw preset name in header
void draw_preset_name(void) {
  font_string_region_clip(headRegion, "                  ", 64, 0, 0, 0);
  font_string_region_clip(headRegion, preset_name((u8)preset_get_select()), 64, 0, 0x5, 0);
  headRegion->dirty = 1;
}

// draw to the head region
void notify(const char* str) {
    region_fill(headRegion, 0x0);
    font_string_region_clip(headRegion, str, 0, 0, 0xa, 0);
    headRegion->dirty = 1;
    render_update();
}
