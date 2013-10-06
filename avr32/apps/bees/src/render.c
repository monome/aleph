/* render.c
   bees
   aleph
   
   render functions common to pages.

 */


// std
#include <string.h>

#include "print_funcs.h"

// common
#include "fix.h"
#include "simple_string.h"
#include "types.h"

// avr32
#include "app.h"
#include "region.h"
#include "screen.h"

// bees
#include "render.h"

static region* headRegion = NULL;
static region* footRegion = NULL;
static region* scrollRegion = NULL;
static region* selectRegion = NULL;

// check dirty flag and update region
static inline void region_update(region* r) {
  if(r->dirty) {
    screen_draw_region(r->x, r->y, r->w, r->h, r->data);
    r->dirty = 0;
  }
}

//----------------
//--- extern

void render_update(void) {
  app_pause();
  /// draw better
  // region_update(scrollRegion);
  ///
  
  region_update(headRegion);
  region_update(footRegion);
  app_resume();
}

// set current header region
void render_set_head_region(region* reg) {
  headRegion = reg;
  headRegion->dirty = 1;
}

// set current footer region
void render_set_foot_region(region* reg) {
  footRegion = reg;
  footRegion->dirty = 1;
}

// set current scroll region
void render_set_scroll_region(region* reg) {
  scrollRegion = reg;
  scrollRegion->dirty = 1;
}

