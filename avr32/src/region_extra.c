#include "region_extra.h"
#include "font.h"
#include "font_aa.h"

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
