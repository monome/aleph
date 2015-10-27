#include "print_funcs.h"

/* font.c
 *
 * bitmap font data 
 * designed by Henrik Theiling for Liquid Rendering, 2009
 * 
 *
 * adapted for aleph project by monome 2012
*/

// ordering is column-first for easier variable spacing
/* (0,0) = top left
 * pixel(x,y) = (bool)(font_data[x].data & (1 << y)) */

//#include "compiler.h"
#include "types.h"
#include "font.h"

#include "fonts/ume_tgo5_18.h"

// maxiumum string size to attempt rendering
#define MAX_RENDER_STRING 32

// glyph table to use 

/// FIXME: implement this, it would be a little faster
// column-first buffer indexing
// #define COL_FIRST

// glyph.last is the inset from right hand edge of glyph box...
const glyph_t font_data[]= {
  { /* 0x00020UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
  { /* 0x00021UL, 0, 0UL, */ 3, 2, { 0x00, 0x00, 0x00, 0x5e, 0x00, 0x00 } },
  { /* 0x00022UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x06, 0x00, 0x06, 0x00 } },
  { /* 0x00023UL, 0, 0UL, */ 1, 0, { 0x00, 0x24, 0x7e, 0x24, 0x7e, 0x24 } },
  { /* 0x00024UL, 0, 0UL, */ 1, 0, { 0x00, 0x48, 0x54, 0xfe, 0x54, 0x24 } },
  { /* 0x00025UL, 0, 0UL, */ 1, 1, { 0x00, 0x48, 0x20, 0x10, 0x48, 0x00 } },
  { /* 0x00026UL, 0, 0UL, */ 1, 0, { 0x00, 0x34, 0x4a, 0x54, 0x20, 0x50 } },
  { /* 0x00027UL, 0, 0UL, */ 3, 2, { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00 } },
  { /* 0x00028UL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x3c, 0x42, 0x00, 0x00 } },
  { /* 0x00029UL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x42, 0x3c, 0x00, 0x00 } },
  { /* 0x0002aUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x54, 0x38, 0x54, 0x00 } },
  { /* 0x0002bUL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x10, 0x38, 0x10, 0x00 } },
  { /* 0x0002cUL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x80, 0x60, 0x00, 0x00 } },
  { /* 0x0002dUL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x10, 0x10, 0x00, 0x00 } },
  { /* 0x0002eUL, 0, 0UL, */ 3, 2, { 0x00, 0x00, 0x00, 0x40, 0x00, 0x00 } },
  { /* 0x0002fUL, 0, 0UL, */ 1, 2, { 0x00, 0x60, 0x18, 0x06, 0x00, 0x00 } },
  { /* 0x00030UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x3c, 0x42, 0x3c, 0x00 } },
  { /* 0x00031UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x04, 0x7e, 0x00, 0x00 } },
  { /* 0x00032UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x62, 0x52, 0x4c, 0x00 } },
  { /* 0x00033UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x42, 0x4a, 0x36, 0x00 } },
  { /* 0x00034UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x1c, 0x10, 0x7e, 0x00 } },
  { /* 0x00035UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x4e, 0x4a, 0x32, 0x00 } },
  { /* 0x00036UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x3c, 0x4a, 0x32, 0x00 } },
  { /* 0x00037UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x02, 0x72, 0x0e, 0x00 } },
  { /* 0x00038UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x34, 0x4a, 0x34, 0x00 } },
  { /* 0x00039UL, 0, 2UL, */ 2, 1, { 0x00, 0x00, 0x4c, 0x52, 0x3c, 0x00 } },
  { /* 0x0003aUL, 0, 0UL, */ 3, 2, { 0x00, 0x00, 0x00, 0x48, 0x00, 0x00 } },
  { /* 0x0003bUL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x80, 0x64, 0x00, 0x00 } },
  { /* 0x0003cUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x10, 0x28, 0x44, 0x00 } },
  { /* 0x0003dUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x28, 0x28, 0x28, 0x00 } },
  { /* 0x0003eUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x44, 0x28, 0x10, 0x00 } },
  { /* 0x0003fUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x52, 0x0a, 0x04, 0x00 } },
  { /* 0x00040UL, 0, 0UL, */ 1, 0, { 0x00, 0x7c, 0x82, 0x92, 0xaa, 0x3c } },
  { /* 0x00041UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7c, 0x12, 0x7c, 0x00 } },
  { /* 0x00042UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x4a, 0x34, 0x00 } },
  { /* 0x00043UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x3c, 0x42, 0x42, 0x00 } },
  { /* 0x00044UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x42, 0x3c, 0x00 } },
  { /* 0x00045UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x4a, 0x42, 0x00 } },
  { /* 0x00046UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x0a, 0x02, 0x00 } },
  { /* 0x00047UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x3c, 0x42, 0x72, 0x00 } },
  { /* 0x00048UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x08, 0x7e, 0x00 } },
  //  { /* 0x00049UL, 0, 0UL, */ 3, 2, { 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00 } },
  // there are 2 uppercase I's... this one has serifs
  { /* 0x00049UL, 0, 3UL, */ 2, 1, { 0x00, 0x00, 0x42, 0x7e, 0x42, 0x00 } },
  { /* 0x0004aUL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x80, 0x7e, 0x00, 0x00 } },
  { /* 0x0004bUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x08, 0x76, 0x00 } },
  { /* 0x0004cUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x40, 0x40, 0x00 } },
  { /* 0x0004dUL, 0, 0UL, */ 1, 0, { 0x00, 0x7e, 0x04, 0x08, 0x04, 0x7e } },
  { /* 0x0004eUL, 0, 0UL, */ 1, 1, { 0x00, 0x7e, 0x04, 0x08, 0x7e, 0x00 } },
  { /* 0x0004fUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x3c, 0x42, 0x3c, 0x00 } },
  { /* 0x00050UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x12, 0x0c, 0x00 } },
  { /* 0x00051UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x3c, 0xc2, 0xbc, 0x00 } },
  { /* 0x00052UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x12, 0x6c, 0x00 } },
  { /* 0x00053UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x44, 0x4a, 0x32, 0x00 } },
  { /* 0x00054UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x02, 0x7e, 0x02, 0x00 } },
  { /* 0x00055UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x3e, 0x40, 0x7e, 0x00 } },
  { /* 0x00056UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x20, 0x1e, 0x00 } },
  { /* 0x00057UL, 0, 0UL, */ 1, 0, { 0x00, 0x3e, 0x40, 0x30, 0x40, 0x3e } },
  { /* 0x00058UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x66, 0x18, 0x66, 0x00 } },
  { /* 0x00059UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x0e, 0x70, 0x0e, 0x00 } },
  { /* 0x0005aUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x62, 0x5a, 0x46, 0x00 } },
  { /* 0x0005bUL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x7e, 0x42, 0x00, 0x00 } },
  { /* 0x0005cUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x06, 0x18, 0x60, 0x00 } },
  { /* 0x0005dUL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x42, 0x7e, 0x00, 0x00 } },
  { /* 0x0005eUL, 0, 0UL, */ 1, 0, { 0x00, 0x08, 0x04, 0x7e, 0x04, 0x08 } },
  { /* 0x0005fUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x80, 0x80, 0x80, 0x00 } },
  { /* 0x00060UL, 0, 0UL, */ 1, 1, { 0x00, 0x48, 0x7c, 0x4a, 0x42, 0x00 } },
  { /* 0x00061UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x20, 0x54, 0x78, 0x00 } },
  { /* 0x00062UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x44, 0x38, 0x00 } },
  { /* 0x00063UL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x38, 0x44, 0x00, 0x00 } },
  { /* 0x00064UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x38, 0x44, 0x7e, 0x00 } },
  { /* 0x00065UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x38, 0x54, 0x4c, 0x00 } },
  { /* 0x00066UL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x7c, 0x0a, 0x00, 0x00 } },
  { /* 0x00067UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x98, 0xa4, 0x7c, 0x00 } },
  { /* 0x00068UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x04, 0x78, 0x00 } },
  { /* 0x00069UL, 0, 0UL, */ 3, 2, { 0x00, 0x00, 0x00, 0x7a, 0x00, 0x00 } },
  { /* 0x0006aUL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x80, 0x7a, 0x00, 0x00 } },
  { /* 0x0006bUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7e, 0x18, 0x64, 0x00 } },
  { /* 0x0006cUL, 0, 0UL, */ 3, 2, { 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00 } },
  { /* 0x0006dUL, 0, 0UL, */ 1, 0, { 0x00, 0x7c, 0x04, 0x7c, 0x04, 0x78 } },
  { /* 0x0006eUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7c, 0x04, 0x78, 0x00 } },
  { /* 0x0006fUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x38, 0x44, 0x38, 0x00 } },
  { /* 0x00070UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0xfc, 0x44, 0x78, 0x00 } },
  { /* 0x00071UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x38, 0x24, 0xfc, 0x00 } },
  { /* 0x00072UL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x78, 0x04, 0x00, 0x00 } },
  { /* 0x00073UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x48, 0x54, 0x24, 0x00 } },
  { /* 0x00074UL, 0, 0UL, */ 2, 2, { 0x00, 0x00, 0x3e, 0x44, 0x00, 0x00 } },
  { /* 0x00075UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x3c, 0x40, 0x7c, 0x00 } },
  { /* 0x00076UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x7c, 0x20, 0x1c, 0x00 } },
  { /* 0x00077UL, 0, 0UL, */ 1, 0, { 0x00, 0x3c, 0x40, 0x38, 0x40, 0x3c } },
  { /* 0x00078UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x6c, 0x10, 0x6c, 0x00 } },
  { /* 0x00079UL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x1c, 0xa0, 0x7c, 0x00 } },
  { /* 0x0007aUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x64, 0x54, 0x4c, 0x00 } },
  { /* 0x0007bUL, 0, 0UL, */ 2, 1, { 0x00, 0x00, 0x08, 0x76, 0x42, 0x00 } },
  { /* 0x0007cUL, 0, 0UL, */ 3, 2, { 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00 } },
  { /* 0x0007dUL, 0, 0UL, */ 1, 2, { 0x00, 0x42, 0x76, 0x08, 0x00, 0x00 } },
  { /* 0x0007eUL, 0, 0UL, */ 1, 1, { 0x00, 0x04, 0x02, 0x04, 0x02, 0x00 } },
};

const U32 font_nglyphs = sizeof(font_data)/sizeof(glyph_t) - 1;



//------------------------------------------
//-----  functions


// render single glyph to a flat buffer (1byte = 1px)
// given pointer to buffer, pixel offset, row length,
// foreground and background colors
// return columns used
extern u8 font_glyph(char ch, u8* buf, u8 w, u8 a, u8 b) {
  u8 i=0;
  u8 j;
  u8 * p = buf;
  const glyph_t* gl = &(font_data[ch - FONT_ASCII_OFFSET]);

  // columns to draw
  u8 cols = FONT_CHARW - gl->first - gl->last;
  while(i < cols) {
    for(j=0; j<FONT_CHARH; j++) {
      *p = gl->data[i + gl->first] & (1 << j) ? a : b;
      //      if(*buf) { print_dbg("#"); } else { print_dbg("_"); }
      // point at next row
      p += w;
    }
    // increment column count
    i++;
    // reset pointer to row
    p = buf + i;
  }
  return cols;
}

// fixed_width variant
extern u8 font_glyph_fixed(char ch, u8* buf, u8 w, u8 a, u8 b) {
  u8 i=0;
  u8 j;
  u8 * p = buf;
  const glyph_t* gl = &(font_data[ch - FONT_ASCII_OFFSET]);

  // columns to draw
  while(i < FONT_CHARW) {
    for(j=0; j<FONT_CHARH; j++) {
      *p = gl->data[i + gl->first] & (1 << j) ? a : b;
      // point at next row
      p += w;
    }
    // increment column count
    i++;
    // reset pointer to row
    p = buf + i;
  }
  return FONT_CHARW;
}

// same as font_glyph, double size
extern u8* font_glyph_big(char ch, u8* buf, u8 w, u8 a, u8 b) {
  u8 i=0, j, val;
  u8* p = buf;
  const glyph_t* gl = &(font_data[ch - FONT_ASCII_OFFSET]);
  // columns to draw
  u8 cols = (FONT_CHARW - gl->first - gl->last);
  while(i < cols) {
    for(j=0; j<FONT_CHARH; j++) {
      val = gl->data[i + gl->first] & (1 << j) ? a : b;
      *p = val;
      *(p +1) = val;
      // point at next row
      p += w;
      // fill the next row as well
      *p = val;
      *(p +1) = val;
      // point at next row
      p += w;
    }
    // move pointer back the size of one column
    //    buf -= colOffset;
    // increment for next row
    //    buf += 2;
    // increment column count
    i++;
    // set pointer to next (pixel*2) in first row
    p = buf + (i*2);
  }
  return p;
}

// same as font_glyph,m 4x size
extern u8* font_glyph_bigbig(char ch, u8* buf, u8 w, u8 a, u8 b) {
  u8 i=0, j, val;
  u8* p = buf;
  const glyph_t* gl = &(font_data[ch - FONT_ASCII_OFFSET]);
  // columns to draw
  u8 cols = (FONT_CHARW - gl->first - gl->last);
  while(i < cols) {
    for(j=0; j<FONT_CHARH; j++) {
      val = gl->data[i + gl->first] & (1 << j) ? a : b;
      *p = val;
      *(p +1) = val;
      *(p +2) = val;
      *(p +3) = val;
      // point at next row
      p += w;
      // fill the next row as well
      *p = val;
      *(p +1) = val;
      *(p +2) = val;
      *(p +3) = val;
      // point at next row
      p += w;
      // fill the next row as well
      *p = val;
      *(p +1) = val;
      *(p +2) = val;
      *(p +3) = val;
      // point at next row
      p += w;
      // fill the next row as well
      *p = val;
      *(p +1) = val;
      *(p +2) = val;
      *(p +3) = val;
      // point at next row
      p += w;
    }
    // move pointer back the size of one column
    //    buf -= colOffset;
    // increment for next row
    //    buf += 2;
    // increment column count
    i++;
    // set pointer to next (pixel*2) in first row
    p = buf + (i << 2);
  }
  return p;
}


// render a string of packed glyphs to a buffer
u8* font_string(const char* str, u8* buf, u32 size, u8 w, u8 a, u8 b) {
  u8* max = buf + size - 8; // pad 1 character width on right edge
  //  u32 x = 0;
  //  u8 dx = 0;

  while(buf < max) {
  //  while (x < w) {
    if (*str == 0) {
      // end of string
      break;
    }
    //    dx = font_glyph(*str, buf, w, a, b);
    //    buf = +=dx;
    buf += font_glyph(*str, buf, w, a, b);
    //    x += dx;
    // 1-column space between chars
    ++buf;
    //    ++x;
    ++str;
  }
  return buf;
}


// render a font string to a region.
// this allows for smarter bounds handling
void font_string_region_wrap(region* reg, const char* str, u8 xoff, u8 yoff, u8 fg, u8 bg) {
  u8* buf = reg->data + xoff + (u32)(reg->w) * (u32)yoff;
  u8* max = reg->data + reg->len;
  u32 xmax = reg->w - 7; // padding
  u8 dx = 0;
  while(buf < max) {
    // break on end of string
    if(*str == 0) { break; }    
    dx = font_glyph(*str, buf, reg->w, fg, bg) + 1;
    buf += dx;
    xoff += dx;
    ++str;
    // wrap lines
    if(xoff > xmax) { 
      xoff = 0; 
      buf += (reg->w - xoff); 
      print_dbg("\r\n font_string_region: wrapped line");
    }
  } 
}

// clipping variant
void font_string_region_clip(region* reg, const char* str, u8 xoff, u8 yoff, u8 fg, u8 bg) {
  u8* buf = reg->data + xoff + (u32)(reg->w) * (u32)yoff;
  u8* max = reg->data + reg->len;
  u32 xmax = reg->w - 7; // padding
  u8 dx = 0;
  while(buf < max) {
    // break on end of string
    if(*str == 0) { break; }    
    dx = font_glyph(*str, buf, reg->w, fg, bg) + 1;
    buf += dx;
    xoff += dx;
    ++str;
    // wrap lines
    if(xoff > xmax) { 
      return; 
    }
  } 
}


// same as font_string, double size
u8* font_string_big(const char* str, u8* buf, u32 size, u8 w, u8 a, u8 b) {
  u8* max = buf + size;
  while (buf < max) {
    if (*str == 0) {
      // end of string
      break;
    }
    buf = font_glyph_big(*str, buf, w, a, b);
    /* // 1-column space between chars */
    /* buf++; */
    /* str++; */
    // 2-column space between chars
    buf += 2;
    ++str;

  }
  return buf;
}

// same as font_string, 4x size
u8* font_string_bigbig(const char* str, u8* buf, u32 size, u8 w, u8 a, u8 b) {
  u8* max = buf + size;
  while (buf < max) {
    if (*str == 0) {
      // end of string
      break;
    }
    buf = font_glyph_bigbig(*str, buf, w, a, b);
    // 1-column space between chars
    //    buf++;
    // 3-column space
    buf += 3;
    str++;
  }
  return buf;
}






//-------------
//----- anti-aliased fonts (bitmaps)

// render an anti-aliased (4-bit) glyph to a buffer
// arguments are character, buffer, target row size, invert flag
extern u8* font_glyph_aa(char ch, u8* buf, u8 w, u8 inv) {
  //#if 0
  const char* gl; // glyph data
  //  u8 gw, gh; // glyph width and height
  u8 i, j;
  // how many bytes to move buf pointer for 1st column of next row
  u32 nextRowBytes;
    // buf pointer;
  char* p;
  // pointer to copy function
  //  void (*fp)(const char* src, char* dst);
  
  // fixme: wtf why
  //  if(inv) { fp = &copyPxInv; } else { fp = &copyPx; }

#if 0
  /// FIXME: might want only numerals
  //// hackish
  if( (ch > 45) && (ch < 58)) { // dot + numerals
    gl = FONT_AA[ch - 46].glyph.data;
    // print_dbg("\r\n printing numeral ");
    // print_dbg_char(ch);
    // print_dbg(" , glyphidx: ");
    // print_dbg_ulong(ch - 46);
  } else { 
    return buf;
  }
#else
  //  gl = FONT_AA[ch - FONT_ASCII_OFFSET].glyph.data;
  /// flaarggg
  //  print_dbg("\r\n render glyph: ");
  //  print_dbg_char(ch);
  //  print_dbg(" ( 0x");
  //  print_dbg_hex((u32)ch);
  
  ////////
  /// FIXME: font is missing ` or _ or something
  if(ch > 95) { ch--; }
  ////////
  gl = FONT_AA[ch - FONT_ASCII_OFFSET].glyph.data;
#endif
  /// copy glyph to buffer...
  p = (char*)buf;
  nextRowBytes = w - FONT_AA_CHARW;
  //  print_dbg("\r\n");
  //  print_dbg("\r\n");

  if(inv) {
    // loop over rows
    for(i=0; i<FONT_AA_CHARH; i++) {
      // loop over columns
      for(j=0; j<FONT_AA_CHARW; j++) {
	//	print_dbg_char(*gl + 0x20);
	//      print_dbg(dbgStr[(u32)(*gl)]);
	//	print_dbg(" ");
	// copy/invert pixel and advance pointers
	*p++ = 0xf - *gl++;
      }
      //      print_dbg("\r\n");
      // reset column and advance row
      p += nextRowBytes;
    }
    // return original buf pointer plus glyph width
  } else {
    // loop over rows
    for(i=0; i<FONT_AA_CHARH; i++) {
      // loop over columns
      for(j=0; j<FONT_AA_CHARW; j++) {
	//	print_dbg_char(*gl + 0x20);
	//      print_dbg(dbgStr[(u32)(*gl)]);
	//	print_dbg(" ");
	// copy pixel and advance pointers
	*p++ = *gl++;
      }
      //      print_dbg("\r\n");
      // reset column and advance row
      p += nextRowBytes;
    }
  }
  // return original buf pointer plus glyph width
  return buf + FONT_AA_CHARW;
  //#endif
}

// render a string of packed glyphs to a buffer
extern u8* font_string_aa(const char* str, u8* buf, u32 size, u8 w, u8 inv) {
  u8* max = buf + size;
  // print_dbg("\r\n antialiased string \"");
  // print_dbg(str);
  // print_dbg("\" font w, h: (");
  // print_dbg_ulong(FONT_AA_CHARW);
  // print_dbg(" , ");
  // print_dbg_ulong(FONT_AA_CHARH);
  // print_dbg(" ) , buf w: ");
  // print_dbg_ulong(w);

  /// FIXME: this bounds check looks wrong
  while (buf < max) {
    if (*str == 0) {
      // end of string
      break;
    }
    buf = font_glyph_aa(*str, buf, w, inv);
    // 1-column space between chars
    //    buf++;
    //// cutting the fonts with space included
    //// so that background doesn't bleed through here
    str++;
  }
  return buf;  
}
