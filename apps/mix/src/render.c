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

//--- application headers
#include "ctl.h"
#include "render.h"

//-------------------------------------------------
//----- -static variables


/* 
   the screen-drawing routines in avr32_lib provide "region" object
   a simple 16-bit pixel buffer with width, height, x/y offset, and dirty flag.
   there are also methods for basic fill and text rendering into regions.
*/

// one drawing region for each channel
static region regChan[] = { 
  { .w=64, .h=32, .x=0,  .y=0  },
  { .w=64, .h=32, .x=64, .y=0  },
  { .w=64, .h=32, .x=0,  .y=32 },
  { .w=64, .h=32, .x=64, .y=32 },
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
  // colors
  char fg, bg;
  // stupid way to show channel numbers
  static const char num[4][2] = { "0\0", "1\0", "2\0", "3\0" };
  // text buffer
  static char buf[32];

  // set some colors based on mute flag status
  // colors are 4-bit values
  if(ctl_get_mute(ch)) {
    fg = 10;
    bg = 7;
  } else {
    fg = 15;
    bg = 0;
  }

  // point at the appropriate region
  region* reg = &(regChan[ch]);

  // fill the region with the chosen background
  region_fill(reg, bg);

  // write label in top left 
  // (make it always bright white, ignoring foreground)
  region_string(reg, num[ch], 0, 0, 0xf, bg, 0);

  // build decibel value string
  memset(buf, 32, '\0');
  print_fix16(buf, ctl_get_amp_db(ch));

  // render the decibel value...
  /// with big AA font?
  //  region_string_aa( reg, buf, 0, 0, bg > 0);
  // no, there's no room: use the small system font
  region_string( reg, buf, 0, 20, fg, bg, 0);

  // the render functions set the region's dirty flag,
  // so there's nothing left to do now,
  // except wait for the screen refresh timer to trigger a redraw.
}
