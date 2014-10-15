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
// #define NUMSTRBUF_LEN 17
// #define HEXSTRBUF_LEN 9

// static char numstrbuf[NUMSTRBUF_LEN];
// static char hexstrbuf[HEXSTRBUF_LEN] = "12345678";

/* 
   the screen-drawing routines in avr32_lib provide "region" object
   a simple 16-bit pixel buffer with width, height, x/y offset, and dirty flag.
   there are also methods for basic fill and text rendering into regions.
*/

/* // a region above of the screen for showing text label */
/* static region regLabel = {  */
/*   .w = 128, .h = 24, .x = 0,  .y = 0 */
/* }; */

// a region below the screen for showing numerical data
/* static region regData = {  */
/*   .w = 128, .h = 24, .x = 0,  .y = 24 */
/* }; */

/* // a region for each mute button */
/* static region regMute[4] = {  */
/*   { .w = 32, .h = 16, .x = 0,  .y = 48 }, */
/*   { .w = 128, .h = 24, .x = 32,  .y = 24 }, */
/*   { .w = 128, .h = 24, .x = 64,  .y = 24 }, */
/*   { .w = 128, .h = 24, .x = 96,  .y = 24 }, */
/* }; */

// one drawing region for each channel
static region regChan = { 
  { .w=64, .h=32, .x=0,  .y=0  },
  { .w=64, .h=32, .x=32, .y=0  },
  { .w=64, .h=32, .x=0,  .y=32 },
  { .w=64, .h=32, .x=32, .y=32 },
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
    region_draw(&(regChan[i]), 0x0);
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


static const char* print_db(amp) {
}





// render amplitude
void render_chan(u8 ch) {
  static const char nums[4][1] = { "0", "1", "2", "3" };
  static char buf[32];
  region* reg = &(regChan[ch]);
  // clear the region
  region_fill(reg, 0x0);
  // write label in top left
  region_string(reg, num[ch]);
  // build string
  memset(buf, 32, '\0');
 


  /* static const char nums[4][1] = { "0", "1", "2", "3" }; */
  /* // text buffer */
  /* char buf[16] = ""; */

  /* strcat(buf, "ADC ch "); */
  /* strcat(buf, nums[ch & 3]); */
  /* // render text to label region (large) */
  /* region_string_aa( &regLabel, buf , 0, 0, 0); */
  /* // clear buffer */
  /* memset(buf, 16, '\0'); */

  /* // print value in 16.16 */
  /* // shift it to show only the fractional part (amplitude is 0-1) */
  /* // 15 bits because top part is signed, bottom part unsigned */
  /* print_fix16(buf, ctl_get_amp(ch) >> 15); */
  /* // render to buffer */
  /* region_string_aa( &regData, buf , 0, 0, 0); */

  /* /\* // glitch it up... *\/ */
  /* /\* // clear buffer again *\/ */
  /* /\* memset(buf, 16, '\0'); *\/ */
  /* /\* // print again, in hex *\/ */
  /* /\* uint_to_hex_ascii(buf, val); *\/ */
  /* /\* // render again, to same region, small, inverted and offset *\/ */
  /* /\* region_string( &regData, buf , 0, 64, 1); *\/ */

}

// render mute
void render_mute(u8 ch) {
  /* // fill with black or white depending on mute status (marks changed) */
  /* if(ctl_get_mute(ch)) {  */
  /*   region_fill( &(regMute[ch]), 0xf); */
  /* } else { */
  /*   region_fill( &(regMute[ch]), 0); */
  /* } */
}


