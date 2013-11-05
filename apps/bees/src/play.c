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
  const s16 opIdx = net_in_op_idx(idx);
  region_fill(lineRegion, 0x0);
  if(opIdx >= 0) {
    // operator input
    // build descriptor string
    clearln();
    appendln_idx_lj(opIdx);
    appendln_char('.');
    appendln( net_op_name(opIdx) );
    appendln_char('/');
    appendln( net_in_name(idx) );
    endln();

    font_string_region_clip(lineRegion, lineBuf, 0, 0, 0xa, 0);
    clearln();

    print_fix16(lineBuf, net_get_in_value(idx));
    font_string_region_clip(lineRegion, lineBuf, LINE_VAL_POS, 0, 0xa, 0);
  } else {
    // parameter input    
    clearln();
    appendln_idx_lj( (int)net_param_idx(idx)); 
    appendln_char('.');
    appendln( net_in_name(idx)); 
    endln();
    font_string_region_clip(lineRegion, lineBuf, 0, 0, 0xa, 0);
    clearln();
    print_fix16(lineBuf, net_get_in_value(idx));
    font_string_region_clip(lineRegion, lineBuf, LINE_VAL_POS, 0, 0xa, 0);
  }
   render_to_scroll_bottom();
}
