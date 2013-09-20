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


// hilight a region with given color and threshold
void region_hl(region* reg, u8 c, u8 thresh) {
  u32 i;
  for(i=0; i<reg->len; i++) {
    if ( reg->data[i] < thresh) { reg->data[i] = c; }
  }
  reg->dirty = 1;
}
