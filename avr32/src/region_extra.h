// additional region-drawing methods, missing from libavr32

#ifndef _ALEPH_AVR32_REGION_EXTRA_H_
#define _ALEPH_AVR32_REGION_EXTRA_H_

#include "region.h"

// render a string to a region using the default anti-aliased font.
 extern void region_string_aa(
 		   region* reg,	 // region
 		   const char* str,// string
 		   u8 x, u8 y, 	 // offset
 		   u8 inv ); // inversion flag

#endif
