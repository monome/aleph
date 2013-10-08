#ifndef _RENDER_H_
#define _RENDER_H_

// avr32
#include "region.h"
// bees
#include "menu.h"
#include "menu_protected.h"

// all scroll regions on the pages have the same dimensions.
// selection is always at the center.

// center line
#define SCROLL_CENTER_LINE 	3
// lines above center
#define SCROLL_LINES_BELOW 	3
#define SCROLL_LINES_ABOVE 	4
#define SCROLL_LINES_ABOVE_1 	3

// lines below center
// in bytes, offset for top of center row
#define SCROLL_CENTER_OFFSET 384 // SCROLL_CENTER_LINE * bytes per line

// draw dirty regions to the screen
extern void render_update(void);

// set current header region
void render_set_head_region(region* reg);

// set current footer region
void render_set_foot_region(region* reg);

// set current scroll region
void render_set_scroll_region(region* reg) ;


#endif // header guard
