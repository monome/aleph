// std
#include <string.h>

// asf
#include "print_funcs.h"

// avr32
#include "app.h"
#include "font.h"
#include "memory.h"
#include "screen.h"

// lppr
#include "render.h"

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


//-------------------------------------------------
//----- -static variables

// temp buffer
#define STRBUF_LEN 16

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
//----- -static functions

// allocate buffer
static void region_alloc(region* reg) {
  u32 i;
  reg->len = reg->w * reg->h;
  reg->data = (u8*)alloc_mem(reg->len);
  for(i=0; i<reg->len; i++) {
    reg->data[i] = 0; 
  }
  reg->dirty = 0;
}

/* static void region_free(region* reg) { */
/*   //... haha */
/* } */


// render a string to a region with offset
static inline void region_string(
				 region* reg,	 // region
				 const char* str,// string
				 u8 x, u8 y, 	 // offset
				 u8 a, u8 b, 	 // colors
				 u8 sz)  // size levels (dimensions multiplied by 2**sz)
{
  if(sz == 0) {
    font_string(str, reg->data + (u32)reg->w * (u32)y + (u32)x, reg->len, reg->w, a, b);
  } else if (sz == 1) {
    font_string_big(str, reg->data + (u32)reg->w * (u32)y + (u32)x, reg->len, reg->w, a, b);
  } else if (sz == 2) {
    font_string_bigbig(str, reg->data + (u32)reg->w * (u32)y + (u32)x, reg->len, reg->w, a, b);
  }
  reg->dirty = 1;
}

static void region_clear(region* reg) {
  u32 i;
  for(i=0; i<reg->len; i++) {
    reg->data[i] = 0; 
  }
  reg->dirty = 1;
}

static void region_fill(region* reg, u8 c) {
  u32 i;
  for(i=0; i<reg->len; i++) {
    reg->data[i] = c; 
  }
  reg->dirty = 1;
}
				     


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
  //  region_string(&status, "             ", 32, 16, 0xf, 0x0, 1);
  //  region_clear(&status);
  region_fill(&bigtop, 0x5);
  region_string(&bigtop, "LPPR", 40, 12, 0xf, 0x0, 2);
  region_string(&(foot[0]), "TAP1", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[1]), "TAP2", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[2]), "REC", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[3]), "PLAY", 0, 0, 0xf, 0x0, 0);
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
  // highlight the footer
}

// draw delay time
void render_delay_time(u8 id, u32 ms, u32 samps) {
  static char strbuf[12];
  memset(strbuf, ' ', 12);
  //  itoa_whole(ms, strbuf, 12);
  itoa_whole_lj(ms, strbuf);
  region_string(&bigtop, strbuf, 30, 20, 0xf, 0x0, 1);
  memset(strbuf, ' ', 12);
  //  itoa_whole(samps, strbuf, 12);
  itoa_whole_lj(samps, strbuf);
  region_string(&bigtop, strbuf, 30, 40, 0xf, 0x0, 1);
}
    
// clear the main region when a new knob is touched
/* void render_new_param_touch(void) { */
/*   region_fill(&bigtop, 0x5); */
/* } */

// draw labels for delay time
extern void render_touched_delaytime(u8 id) {
  region_clear(&bigtop);
  region_string(&bigtop, "delay time", 0, 0, 0xa, 0x3, 0 );
  if(id) { region_string(&bigtop, "2", 40, 0, 0xa, 0x3, 1 ); }
  else { region_string(&bigtop, "1", 40, 0, 0xa, 0x3, 1 ); }
  region_string(&bigtop, "ms : ", 	0, 20, 0xd, 0x0, 0 );
  region_string(&bigtop, "samples : ", 	0, 40, 0xd, 0x0, 0 );
  //  region_string(&bigtop, strbuf, 0, 0, 0xf, 0x0, 1);
  //  region_string(&bigtop, strbuf, 0, 17, 0xf, 0x0, 1);

}


#undef LINE_BUF_LEN
