/* regions.h 
   
   draw and otherwise manipulate  arbitrary grids of pixels.

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


// allocate and initialize
void region_alloc(region* reg);

/// ha
/*  void region_free(region* reg) */

// render a string to a region with offset
  void region_string(
		     region* reg,	 // region
		     const char* str,// string
		     u8 x, u8 y, 	 // offset
		     u8 a, u8 b, 	 // colors
		     u8 sz);  // size levels (dimensions multiplied by 2**sz)

// fill a region with given color
void region_fill(region* reg, u8 c);
  

// hilight a region with given color and threshold
void region_hl(region* reg, u8 c, u8 thresh);

#endif // h guardi
