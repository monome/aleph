// std
#include <string.h>

// asf
#include "print_funcs.h"

// avr32
#include "app.h"
#include "font.h"
#include "memory.h"
#include "region.h"
#include "screen.h"

// local
#include "render.h"
//-------------------------------------------------
//----- -static variables

// temp buffer
#define NUMSTRBUF_LEN 16
static char numstrbuf[NUMSTRBUF_LEN];

///// declare screen-drawing regions.
//// len, dirty, and data can be left unitialized aand calculated / allocated in region_init.
// 1 large region filling the screen
//static region bigtop = { .w = 128, .h = 56, .x = 0, .y = 0, };

// dac value regions
static region dacValueReg[4] = {
  { .w = 64, .h = 32, .x = 0, .y = 0 },
  { .w = 64, .h = 32, .x = 0, .y = 32 },
  { .w = 64, .h = 32, .x = 64, .y = 0 },
  { .w = 64, .h = 32, .x = 64, .y = 32 },
};


// scrolling boot region
static region bootScrollRegion = {
  .w = 128, .h = 64, .x = 0, .y = 0
};
// scroller for boot region
static scroll bootScroll;

// array of pointers to all regoins.
// NOTE: regions can overlap,
// but the ordering of this list also determines drawing order.
// later entries will overwrite earlier entries in the esame redraw cycle.
static region * allRegions[] = {
  &(dacValueReg[0]),
  &(dacValueReg[1]),
  &(dacValueReg[2]),
  &(dacValueReg[3]),

};

static const u8 numRegions = 4;

//-------------------------------------------------
//----- external functions


// initialze renderer
void render_init(void) {
  u32 i;

  for(i = 0; i<numRegions; i++) {
    region_alloc((region*)(allRegions[i]));
  }
  // scrolling boot region
  region_alloc((region*)(&bootScrollRegion));
  scroll_init(&bootScroll, &bootScrollRegion);

}

// render text to statusbar
/* void render_status(const char* str) { */
/*   static u32 i; */
/*   for(i=0; i<(status.len); i++) { */
/*     status.data[i] = 0; */
/*   } */
  
/*   region_string(&status, str, 0, 0, 0xf, 0, 0); */
/* } */

// fill with initial graphics (id strings)
void render_startup(void) {

  region_fill(&(dacValueReg[0]), 0x0);
  region_fill(&(dacValueReg[1]), 0x0);
  region_fill(&(dacValueReg[2]), 0x0);
  region_fill(&(dacValueReg[3]), 0x0);
  

}

// update dirty regions
void render_update(void) {
  region* r;  
  u8 i;
  app_pause();
  // scrolling boot region
  if(bootScroll.reg->dirty) {
    scroll_draw(&bootScroll);
  }
  for(i = 0; i<numRegions; i++) {
    r = allRegions[i]; 
    if(r->dirty) {
      screen_draw_region(r->x, r->y, r->w, r->h, r->data);
      r->dirty = 0;
    }
  }
  app_resume();
}

// force a refresh of all regions
void render_force_refresh(void) {
  u8 i;

  // scrolling boot region
  if(bootScroll.reg->dirty) {
    scroll_draw(&bootScroll);
  }

  for(i = 0; i<numRegions; i++) {
    (allRegions[i])->dirty = 1;
  }
  render_update();
}

/*
void render_sw_on(u8 sw, u8 on) {
  //region_fill(&(foot[sw]), on ? 0xf : 0x1);
}
*/

void render_dac(u8 ch, s32 val) {
  region_fill(&(dacValueReg[ch]), 0x0);
  strcpy(numstrbuf, "    ");
  itoa_whole_lj(ch, numstrbuf);
  region_string(&(dacValueReg[ch]), numstrbuf, 0, 0, 0x2, 0x0, 0);
  itoa_whole_lj(val, numstrbuf);
  region_string_aa(&(dacValueReg[ch]), numstrbuf, 0, 8, 1);
}


// render to scrolling boot buffer
void render_boot(char* str) {
  scroll_string_front(&bootScroll, str);
}

