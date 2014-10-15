/*
  render.c
  
  aleph-mix

  definitions for rendering graphics.

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

//--- application headers
#include "ctl.h"
#include "render.h"

//-------------------------------------------------
//----- -static variables

/* 
   the screen-drawing routines in avr32_lib provide the "region" object
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
  // allocate memory for each region
  region_alloc(&regChan[0]);
  region_alloc(&regChan[1]);
  region_alloc(&regChan[2]);
  region_alloc(&regChan[3]);
}

// fill with initial graphics
void render_startup(void) {
  u32 i;
  for(i=0; i<4; i++) { 
    // fill the graphics buffer (with black)
    region_fill(&(regChan[i]), 0x0);
    // physically render the region data to the screen
    region_draw(&(regChan[i]));
  }
}

// update dirty regions
// (this will be called from an application timer)
void render_update(void) {
  app_pause();

  // physically update the screen with each region's data (if changed)
  region_draw( &(regChan[0]) );
  region_draw( &(regChan[1]) );
  region_draw( &(regChan[2]) );
  region_draw( &(regChan[3]) );

  app_resume();
}

// render amplitude
void render_chan(u8 ch) {
  // tmp decibel value
  s32 db;
  // tmp position for left-justify
  int pos=0;
  // stupid way to show channel numbers
  static const char num[4][3] = { "0.", "1.", "2.", "3." };
  // text buffer
  static char buf[32];
  // point at the appropriate region
  region* reg = &(regChan[ch]);

  // clear the region
  region_fill(reg, 1);

  // build decibel value string
  memset(buf, 32, '\0');
  db = ctl_get_amp_db(ch);
  // print "-inf" if very small,
  // otherwise print the value
  // make sure this is a signed comparison
  if(db < (s32)0xffbf0000) {
    memcpy(buf, "-inf\0", 5);
  } else {
    print_fix16(buf, db);
  }

#if 0
  // use the small system font, or...
  region_string( reg, buf, 0, 6, 0xf, 0, 0);
#else
  // ... use the large anti-aliased font.
  // this means we have to truncate the string to make it fit.
  // write an early termination symbol to lose some decimal points..
  buf[9] = '\0';
  // ...and skip leading spaces:
  while(buf[pos] == ' ' && pos < 31) { pos++; }
  if (pos == 31) { pos = 0; }
  // render an anti-aliased string to the region.
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

  // the render functions set the region's dirty flag,
  // so there's nothing left to do now,
  // except wait for the screen refresh timer to trigger a redraw.
}
