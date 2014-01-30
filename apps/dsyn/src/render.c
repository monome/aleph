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
#include "util.h"

// local
#include "render.h"
//-------------------------------------------------
//----- -static variables

// temp buffers
#define NUMSTRBUF_LEN 16
#define HEXSTRBUF_LEN 9

static char numstrbuf[NUMSTRBUF_LEN];
static char hexstrbuf[HEXSTRBUF_LEN] = "12345678";

// FIXME: retarded to have this here
static const char* paramStrings[] = {
  "Gate",		// 0
  "Trig",		// 1
  "Amp",		// 2
  "AmpSus",      	// 3
  "AmpAtkSlew",	// 4
  "AmpDecSlew",	// 5
  "AmpRelSlew",	// 6
  "AmpSusDur",	// 7
  "FreqOff",	// 8
  "FreqOn",	// 9
  "FreqSus",	// 10 
  "FreqAtkSlew",	// 11
  "FreqDecSlew",	// 12
  "FreqRelSlew",	// 13
  "FreqSusDur",	// 14
   "RqOff",		// 15
  "RqOn",		// 16
  "RqSus",		// 17
  "RqAtkSlew",	// 18
  "RqDecSlew",	// 19
  "RqRelSlew",	// 20
  "RqSusDur",	// 21
  "Low",		// 22
  "High",		// 23
  "Band",		// 24
  "Notch",		// 25
  "SvfPre",	// 26
  "FreqEnv",	// 27
  "RqEnv",		// 28
};

///// declare screen-drawing regions.
//// len, dirty, and data can be left unitialized and calculated / allocated in region_init.
// 1 large region filling the screen
static region bigtop = { .w = 128, .h = 56, .x = 0, .y = 0, };

// 4 small squares along the bottom text row of the screen
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

// scrolling boot region
static region bootScrollRegion = {
  .w = 128, .h = 64, .x = 0, .y = 0
};
// scroller for boot region
static scroll bootScroll;

static const u8 numRegions = 6;

//-------------------------
//---- static funcs

// render 16.16 decimal
static void render_fix16(fix16 val, region * pr, u8 x, u8 y, u8 inv) {
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  print_fix16(numstrbuf, val);
  region_string_aa(pr, numstrbuf, x, y, inv);
}

#if 0
// render unsigned integer
static void render_int(s32 val, region * pr, u8 x, u8 y, u8 inv) {
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  itoa_whole_lj(val, numstrbuf);
  region_string_aa(pr, numstrbuf, x, y, inv);
}
#endif

// render 8-character hexadecimal
static void render_hex(u32 val, region * pr, u8 x, u8 y, u8 inv) {
  memset(hexstrbuf, ' ', HEXSTRBUF_LEN);
  uint_to_hex_ascii(hexstrbuf, val);
  region_string_aa(pr, hexstrbuf, x, y, inv);
}


//-------------------------------------------------
//----- external functions


// initialze renderer
void render_init(void) {
  u32 i;

  // standard regions
  for(i = 0; i<numRegions; i++) {
    region_alloc((region*)(allRegions[i]));
  }
  // scrolling boot region
  region_alloc((region*)(&bootScrollRegion));
  scroll_init(&bootScroll, &bootScrollRegion);
}


// render to scrolling boot buffer
void render_boot(char* str) {
  scroll_string_front(&bootScroll, str);  
}


// render text to statusbar
void render_status(const char* str) {
  static u32 i;
  for(i=0; i<(status.len); i++) {
    status.data[i] = 0;
  }  
  region_string(&status, str, 0, 0, 0xf, 0, 0);
}

// fill with initial graphics
void render_startup(void) {

  region_fill(&bigtop, 0x5);
  region_string(&bigtop, "DSYN", 40, 12, 0xf, 0x0, 2);
  region_fill(&(foot[0]), 0x0);
  region_fill(&(foot[1]), 0x0);
  region_fill(&(foot[2]), 0x0);
  region_fill(&(foot[3]), 0x0);
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

  // standard regions
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
  scroll_draw(&bootScroll);
  render_update();
}

// render switch on/off status
void render_sw_on(u8 sw, u8 on) {
  region_fill(&(foot[sw]), on ? 0xf : 0x1);
}

// render parameter indx and value
void render_param(u8 vid, s32 pid, s32 val) {
  //// FIXME: too slow.
  //// should make renderTouched routines as in lppr,
  //// do fills and labels there
  region_fill(&bigtop, 0x0);
  switch(vid) {
  case 0:
    region_string_aa(&bigtop, "voice 1", 0, 0, 1);
    break;
  case 1:
    region_string_aa(&bigtop, "voice 2", 0, 0, 1);
    break;
  case 2:
    region_string_aa(&bigtop, "voice 3", 0, 0, 1);
    break;
  case 3:
    region_string_aa(&bigtop, "voice 4", 0, 0, 1);
    break;
  }
  region_string_aa(&bigtop, paramStrings[pid], 0, 20, 1);
  render_hex(val, &bigtop, 0, 40, 1);
}

// render tempo
void render_tempo(u32 ms) {
  // FIXME: not the absolute fastest, and broken if ms > 30k (tempo < 2bpm!)
  // assume sequencer stages are 16ths, i guess
  const fix16 bpm = fix16_div(fix16_from_int(15000), fix16_from_int(ms));
  //// FIXME: should make renderTouched routines as in lppr, 
  //// do fills and labels there
  region_fill(&bigtop, 0x0);
  region_string_aa(&bigtop, "tempo", 0, 0, 1);
  region_string(&bigtop, "(bpm)", 50, 0, 0xf, 0x0, 0);
  //  print_fix16(numstrbuf, bpm);
  render_fix16(bpm, &bigtop, 0, 20, 1);
}

void render_file_write(u8 done) {
  region_fill(&bigtop, 0x0);
  if(done) {
    region_string_aa(&bigtop, "saving params...", 0, 0, 0);
  } else {
    region_string_aa(&bigtop, "saved", 0, 0, 0);
  }
}
