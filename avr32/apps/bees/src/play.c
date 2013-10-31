/* play.c
   bees
   aleph

   implementation for play mode UI
 */

// std
#include "string.h"
// common
#include "fix.h"
// aleph-avr32
#include "print_funcs.h"
#include "region.h"
// bees
#include "net.h"
#include "pages.h"
#include "play.h"
#include "render.h"

// scroll region
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };

// scroll manager
static scroll centerScroll;

// initialize
extern void play_init(void) {
  // allocate regions
  region_alloc(&scrollRegion);
  // init scroll
  scroll_init(&centerScroll, &scrollRegion);
  // fill regions
  region_fill(&scrollRegion, 0x0);
}


// enable rendering (play modal page was selected) 
extern void play_enable_render(void) {
  render_set_scroll(&centerScroll);
}

// process input in play mode
extern void play_input(u16 idx) {
}

