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
static region bigtop = { .w = 128, .h = 56, .x = 0, .y = 0, };

// 4 small squares along the bottom text row of the scree
static region foot[4] = {
  //  { .w = 64, .h = 28, .len =1792, .x =  0, .y =  0, .dirty = 0 } ,
  { .w = 32, .h = 8, .x = 0,  .y = 56, },
  { .w = 32, .h = 8, .x = 32, .y = 56, },
  { .w = 32, .h = 8, .x = 64, .y = 56, },
  { .w = 32, .h = 8, .x = 96, .y = 56, },
};

// long system status bar at top of screen
static region status = {
  //  .w = 128, .h = 8, .len = 1024, .x = 0, .y = 0
  .w = 128, .h = 8, .x = 0, .y = 0
};

// array of pointers to all regoins.
// NOTE: regions can overlap,
// but the ordering of this list also determines drawing order.
// later entries will overwrite earlier entries in the esame redraw cycle.
static region * allRegions[] = {
  &status,
  &bigtop,
  &(foot[0]),
  &(foot[1]),
  &(foot[2]),
  &(foot[3]),
};

static const u8 numRegions = 6;


//-------------------------------------------------
//----- external functions


// initialze renderer
void render_init(void) {
  u32 i;

  for(i = 0; i<numRegions; i++) {
    region_alloc((region*)(allRegions[i]));
  }

  //  screen_clear();

  // test
  print_dbg("\r\n\r\n regions:");
  for(i = 0; i<numRegions; i++) {
    print_dbg("\r\n ( ");
    print_dbg_hex(i);
    print_dbg(" ) @ 0x");
    print_dbg_hex((u32)(allRegions[i]));
    print_dbg(", data: @ 0x");
    print_dbg_hex((u32)(allRegions[i]->data));
    print_dbg(", w:");
    print_dbg_ulong((u32)(allRegions[i]->w));
    print_dbg(", h:");
    print_dbg_ulong((u32)(allRegions[i]->h));
    print_dbg(", len: 0x");
    print_dbg_hex((u32)(allRegions[i]->len));
  }
}

// render text to statusbar
void render_status(const char* str) {
  static u32 i;
  for(i=0; i<(status.len); i++) {
    status.data[i] = 0;
  }
  
  region_string(&status, str, 0, 0, 0xf, 0, 0);
}

// fill with initial graphics (id strings)
void render_startup(void) {

  region_fill(&bigtop, 0x5);
  region_string(&bigtop, "DSYN", 40, 12, 0xf, 0x0, 2);
  //  region_string(&(foot[0]), "TAP1", 0, 0, 0xf, 0x0, 0);
  //  region_string(&(foot[1]), "TAP2", 0, 0, 0xf, 0x0, 0);
  //  region_string(&(foot[2]), "REC", 0, 0, 0xf, 0x0, 0);
  //  region_string(&(foot[3]), "PLAY", 0, 0, 0xf, 0x0, 0);
}

// update dirty regions
void render_update(void) {
  region* r;  
  u8 i;
  app_pause();

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
  for(i = 0; i<numRegions; i++) {
    (allRegions[i])->dirty = 1;
  }
  render_update();
}


void render_sw_on(u8 sw, u8 on) {
  region_fill(&(foot[sw]), on ? 0xf : 0x1);
}

void render_dac(u8 ch, s32 val) {
  /// FIXME: hella stupid to call a fill from a frequently-triggered handler
  region_fill(&bigtop, 0x0);
  strcpy(numstrbuf, "   ");
  itoa_whole(ch, numstrbuf, NUMSTRBUF_LEN);
  region_string(&bigtop, numstrbuf, 0, 0, 0xa, 0x1, 1);
  itoa_whole(val, numstrbuf, NUMSTRBUF_LEN);
  region_string(&bigtop, numstrbuf, 5, 20, 0xf, 0x0, 1);
}

#undef LINE_BUF_LEN
