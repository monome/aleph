#include "font.h"
#include "memory.h"
#include "screen.h"
#include "region.h"


// allocate buffer
void region_alloc(region* reg) {
  u32 i;
  reg->len = reg->w * reg->h;
  reg->data = (u8*)alloc_mem(reg->len);
  for(i=0; i<reg->len; i++) {
    reg->data[i] = 0; 
  }
  reg->dirty = 0;
}

/*  void region_free(region* reg) { */
/*   //... haha */
/* } */

// render a string to a region with offset
void region_string(
		   region* reg,	 // region
		   const char* str,// string
		   u8 x, u8 y, 	 // offset
		   u8 a, u8 b, 	 // colors
		   u8 sz)  // size levels (dimensions multiplied by 2**sz)
{
  if(sz == 0) {
    font_string(str, reg->data + (u32)reg->w * (u32)y + (u32)x, reg->len, reg->w, a, b);
  } else if (sz == 1) {
    font_string_big(str, reg->data + (u32)reg->w * (u32)y + (u32)x, reg->len, reg->w, a, b);
  } else if (sz == 2) {
    font_string_bigbig(str, reg->data + (u32)reg->w * (u32)y + (u32)x, reg->len, reg->w, a, b);
  }
  reg->dirty = 1;
}


// render a string to a region using the default anti-aliased font.
void region_string_aa(
		   region* reg,	 // region
		   const char* str,// string
		   u8 x, u8 y, 	 // offset
		   u8 inv ) // inversion flag
 
{
  font_string_aa(str, reg->data + (u32)reg->w * (u32)y + (u32)x, reg->len, reg->w, inv);
  reg->dirty = 1;
}


// fill a region with given color
void region_fill(region* reg, u8 c) {
  u32 i;
  for(i=0; i<reg->len; i++) {
    reg->data[i] = c; 
  }
  reg->dirty = 1;
}

// fill a contiguous portion of a region with given color
extern void region_fill_part(region* reg, u32 start, u32 len, u8 color) {
 u32 i;
 u8* p = (reg->data) + start;
  for(i=0; i<len; i++) {
    *p++ = color;
  }
  reg->dirty = 1;
}


// hilight a region with given color and threshold
void region_hl(region* reg, u8 c, u8 thresh) {
  u32 i;
  for(i=0; i<reg->len; i++) {
    if ( reg->data[i] < thresh) { reg->data[i] = c; }
  }
  reg->dirty = 1;
}

///---- scrolling stuff

// initialize a text scroller at memory
extern void scroll_init(scroll* scr, region* reg) {
  scr->reg = reg;
  scr->lineCount = 0;
  scr->yOff = 0;
  scr->byteOff = 0;
  scr->lineBytes = FONT_CHARH * reg->w;
  scr->lineCount = reg->h / FONT_CHARH;
  scr->drawSpace = 0;
  // offset of last row
  scr->maxByteOff = reg->w * reg->h - scr->lineBytes;
  region_fill(reg, 0x0);
}

// render text to front of scroll
extern void scroll_string_front(scroll* scr, char* str) {
  /// clear current line
  region_fill_part(scr->reg, scr->byteOff, scr->lineBytes, 0x0);
  // draw text to region at current offset, using system font
  region_string(scr->reg, str,
		0, scr->yOff, 0xf, 0, 0);
  // advance byte offset by count of pixels in row
  scr->byteOff += scr->lineBytes;
  scr->yOff += FONT_CHARH;
  // wrap
  if(scr->yOff >= scr->reg->h) {
    scr->yOff = 0;
  }
  if(scr->byteOff > scr->maxByteOff) {
    scr->byteOff = 0;
  }
  //// render happens separately,
  //  so we can e.g. trigger from timer based on dirty flag
  scr->reg->dirty = 1;
}

// render text to back of scroll
extern void scroll_string_back(scroll* scr, char* str) {
  // temp because we are going to move the offset backwards
  s32 byteoff;
  s8 yoff;
  /// first, decrement the offsets...
  byteoff = scr->byteOff = scr->lineBytes;
  if(byteoff < 0) { byteoff += scr->reg->len; }
  scr->byteOff = byteoff;

  yoff = scr->yOff - FONT_CHARH;
  if (yoff < 0) { yoff += scr->reg->h; }
  scr->yOff = yoff;

  // draw text to region at current offset, using system font
  region_string(scr->reg, str,
		0, scr->yOff, 0xf, 0, 0);
  //// render happens separately,
  //  so we can e.g. trigger from timer based on dirty flag
  scr->reg->dirty = 1;
}



// draw pixels to front of scroll
void scroll_data_front(scroll* scr, u8* data) {
  
}

// draw pixel to back of scroll
void scroll_data_back(scroll* scr, u8* data) {

}




// draw scroll to screen
extern void scroll_draw(scroll* scr) {
  screen_draw_region_offset(0, 0, scr->reg->w, scr->reg->h, scr->reg->len, 
			    scr->reg->data, scr->byteOff + scr->drawSpace);
  scr->reg->dirty = 0;
}
