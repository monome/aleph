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

///// declare screen-drawing regions.
//// len, dirty, and data can be left unitialized aand calculated / allocated in region_init.
// 1 large region filling the screen
static region full = { .w = 128, .h = 64, .x = 0, .y = 0, };

// 4 small squares along the bottom text row of the scree
static region foot[4] = {
  //  { .w = 64, .h = 28, .len =1792, .x =  0, .y =  0, .dirty = 0 } ,
  { .w = 32, .h = 8, .x = 0, .y = 56, },
  { .w = 32, .h = 8, .x = 32, .y = 56,  },
  { .w = 32, .h = 8, .x = 64, .y = 56,  },
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
static region * const allRegions[] = {
  &status,
  &full,
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
  reg->dirty = 1; 
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

//-------------------------------------------------
//----- external functions


// initialze renderer
void render_init(void) {
  u32 i;

  for(i = 0; i<numRegions; i++) {
    region_alloc((region*)allRegions[i]);
  }

  screen_clear();

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
  //  region_string(&(quad[0]), "Q1", 0, 0xf, 0x0);
  //    region_string(&(quad[1]), "Q2", 0, 0xf, 0x0);
  //  region_string(&(quad[2]), "Q3", 0, 0xf, 0x0);
  //  region_string(&(quad[3]), "Q4", 0, 0xf, 0x0);

  /* font_string("box1", (&(quad[0]))->data,  (&(quad[0]))->len,  (&(quad[0]))->w, 0xf, 0x0); */
  /* quad[0].dirty = 1; */

  /* font_string_big("box2", (&(quad[1]))->data,  (&(quad[1]))->len,  (&(quad[1]))->w, 0xf, 0x0); */
  /* quad[1].dirty = 1; */

  /* font_string_bigbig("box3", (&(quad[2]))->data,  (&(quad[2]))->len,  (&(quad[2]))->w, 0xf, 0x0); */
  /* quad[2].dirty = 1; */


  /* font_string("box4", (&(quad[3]))->data,  (&(quad[3]))->len,  (&(quad[3]))->w, 0xf, 0x0); */
  /*   quad[3].dirty = 1;  */

  region_string(&(foot[0]), "TAP1", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[1]), "TAP2", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[2]), "REC", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[3]), "PLAY", 0, 0, 0xf, 0x0, 0);

  /* /// debug: print buffer contents */
  /* print_dbg("\r\n printing quad 0 buffer contents: "); */
  /* for(i=0; i<QUAD_SIZE; i++) { */
  /*   if( (i % QUAD_W) == 0) { */
  /*     print_dbg("\r\n"); */
  /*   } */
  /*   if(quadRegion[0][i] > 0x5) { */
  /*     print_dbg("#");  */
  /*   } else { */
  /*     print_dbg("_"); */
  /*   } */
  /* } */
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
  
}

#undef LINE_BUF_LEN
