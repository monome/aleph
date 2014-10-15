/*
  render.c
  
  aleph-mix

  rendering definitions for mixing module.

  draws amplitude values and mute flags.

*/

//--- std headers
#include <string.h>

//--- asf headers
#include "print_funcs.h"

//--- avr32-lib headers
#include "app.h"
// font-rendering functions
#include "font.h"
// region class
#include "region.h"
// screen refresh function
#include "screen.h"

//--- mix application headers
#include "ctl.h"


// local
#include "render.h"
//-------------------------------------------------
//----- -static variables

// temp buffers
#define NUMSTRBUF_LEN 17
#define HEXSTRBUF_LEN 9

//static char numstrbuf[NUMSTRBUF_LEN];
//static char hexstrbuf[HEXSTRBUF_LEN] = "12345678";

/* 
   the screen-drawing routines in avr32_lib provide "region" object
   a simple 16-bit pixel buffer with width, height, x/y offset, and dirty flag.
   there are also methods for basic fill and text rendering into regions.
*/

// one drawing region for each channel
static region regChan[] = { 
  { .w=62, .h=30, .x=0,  .y=0  },
  { .w=62, .h=30, .x=64, .y=0  },
  { .w=62, .h=30, .x=0,  .y=32 },
  { .w=62, .h=30, .x=64, .y=32 },
};

//-------------------------
//---- static funcs

//... nothing here.

//-------------------------------------------------
//----- external functions

// initialze renderer
void render_init(void) {
  //  u32 i;
  // allocate memory for each region
  region_alloc(&regData);
  region_alloc(&regLabel);
  region_alloc( &(regMute[0]) );
  region_alloc( &(regMute[1]) );
  region_alloc( &(regMute[2]) );
  region_alloc( &(regMute[3]) );
}

// fill with initial graphics
void render_startup(void) {
  region_fill(&regLabel, 0);
  region_string(&regLabel, "MIX", 40, 12, 0xf, 0x0, 2);
  region_fill(&regData, 0x5);
  region_fill(&(regMute[0]), 0x0);
  region_fill(&(regMute[1]), 0x0);
  region_fill(&(regMute[2]), 0x0);
  region_fill(&(regMute[3]), 0x0);
  
  // physically update the screen with each region's data
  region_draw(&regData);
  region_draw(&regLabel);
  region_draw( &(regMute[0]) );
  region_draw( &(regMute[1]) );
  region_draw( &(regMute[2]) );
  region_draw( &(regMute[3]) );
}

// update dirty regions
void render_update(void) {
  //  region* r;  
  //  u8 i;
  app_pause();

  // physically update the screen with each region's data (if changed)
  region_draw(&regData);
  region_draw(&regLabel);
  region_draw( &(regMute[0]) );
  region_draw( &(regMute[1]) );
  region_draw( &(regMute[2]) );
  region_draw( &(regMute[3]) );

  app_resume();
}

// render amplitude
void render_chan(u8 ch) {
  // tmp decibel value
  s32 db;
  // tmp position for left-justify
  int pos=0;
  // stupid way to show channel numbers
  static const char num[4][3] = { "1.", "2.", "3.", "4." };
  // text buffer
  static char buf[32];
  // point at the appropriate region
  region* reg = &(regChan[ch]);

  // clear the region
  region_fill(reg, 1);

  // build decibel value string
  memset(buf, 32, '\0');
  db = ctl_get_amp_db(ch);
  if(db < 0xffbf0000) {
  // print "-inf" if very small
    memcpy(buf, "-inf\0", 5);
  } else {
    // otherwise print the value
    print_fix16(buf, db);
  }

#if 0
  // use the small system font, or...
  region_string( reg, buf, 0, 6, 0xf, 0, 0);
#else
  // ... use the large anti-aliased font.
  // this means we have to truncate the string to make it fit.
  // write an early termination symbol to lose some decimal points:
  buf[9] = '\0';
  // let's also apply an offset that skips leading spaces:
  while(buf[pos] == ' ' && pos < 31) { pos++; }
  if (pos == 31) { pos = 0; }
  region_string_aa( reg, buf + pos, 5, 5, 1);
#endif

  // if channel is muted,
  // highlight background, limit the foreground, 
  // write a big inverted M
  if(ctl_get_mute(ch)) {
    region_hl(reg, 2, 2);
    region_max(reg, 6);
    region_string_aa( reg, "M", 50, 5, 0);
  }

  // write label in small font
  region_string(reg, num[ch], 0, 0, 0xf, 1, 0);

// render mute
void render_mute(u8 ch) {
  // fill with black or white depending on mute status (marks changed)
  if(ctl_get_mute(ch)) { 
    region_fill( &(regMute[ch]), 0xf);
  } else {
    region_fill( &(regMute[ch]), 0);
  }
}
