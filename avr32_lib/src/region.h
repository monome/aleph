/* regions.h 
   
   draw and otherwise manipulate arbitrary grids of pixels.

 */

#ifndef _ALEPH_AVR32_SCREEN_REGION_H_
#define _ALEPH_AVR32_SCREEN_REGION_H_

#include "types.h"

// data type for screen regions
typedef struct _region { 
  // width
  u8 w;
  // height
  u8 h;
  // size (store for speed)
  u32 len;
  // x offset
  u8 x;
  // y offset
  u8 y;  
  // dirty flag
  u8 dirty;
  // data
  u8 * data;
} region;

// datatype for a text scroller
typedef struct _scroll {
  // pointer to region
  region* reg;
  // current byte offset into region
  u32 byteOff;
  // pixels per line of text
  u32 lineBytes;
  // max byte offset before wrapping
  u32 maxByteOff;
  // height of a single row of text
  u8 lineH;
  // current line offset
  //  u8 lineOff;
  // faster to use y-offset in px
  u32 yOff;
  // how many lines of text will fit in the region
  u8 lineCount; 
  // offset for actual rendering:
  // if zero(default), most recent text on last line
  u32 drawSpace;
} scroll;

// allocate and initialize a text scroller
extern void scroll_init(scroll* scr, region* reg);
// draw text to front of scroll
extern void scroll_string_front(scroll* scr, char* str);
// draw text to back of scroll
extern void scroll_string_back(scroll* scr, char* str);
// draw pixels to front of scroll
/// assumes data has correct dimensions!
extern void scroll_region_front(scroll* scr, region* reg);
// draw pixel to back of scroll
/// assumes data has correct dimensions!
extern void scroll_region_back(scroll* scr, region* reg);

// draw scroll to screen
extern void scroll_draw(scroll* scr);
 
// allocate and initialize a screen region
extern void region_alloc(region* reg);

/// ha
/*  void region_free(region* reg) */

// render a string to a region with offset, using system font
extern void region_string(
		   region* reg,	 // region
		   const char* str,// string
		   u8 x, u8 y, 	 // offset
		   u8 a, u8 b, 	 // colors
		   u8 sz);  // size levels (dimensions multiplied by 2**sz)

// render a string to a region using the default anti-aliased font.
extern void region_string_aa(
		   region* reg,	 // region
		   const char* str,// string
		   u8 x, u8 y, 	 // offset
		   u8 inv ); // inversion flag
 
// fill a region with given color
extern void region_fill(region* reg, u8 c);
// fill a contiguous portion of a region with given color
extern void region_fill_part(region* reg, u32 start, u32 len, u8 color );

/// copy-or
  

// hilight a region with given color and threshold
extern void region_hl(region* reg, u8 c, u8 thresh);


#endif // h guard
