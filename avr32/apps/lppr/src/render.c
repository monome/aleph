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

// lppr
#include "inputs.h"
#include "render.h"

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
// string buffer for numerics
#define NUMSTRBUF_LEN 10
static char numstrbuf[NUMSTRBUF_LEN];


/////////////
//// FIXME: scrolling experiments, need cleanup
// scrolling region for boot messages (not refreshed from render callback)
static region bootRegion = { .w = 128, .h=64, .x=0, .y=0 };
// how many items have we stuck in the scroller
static u32 bootCount = 0;
// byte offset into boot-display region
static u32 bootByteOffset = 0;
// row offset into boot-display region
static u32 bootRowOffset = 0;
// how many lines of text fit in the scroll buf
static const u32 bootRowCount = 8; // SCREEN_COL_PX / FONT_CHARH
// how many pixels per line of system text
static const u32 bootRowBytes = 1024; // screen_row_px * font_char_h
// max offset before wrapping
static const u32 bootMaxByteOffset = 7296; // offset of last row

//-------------------------------------------------
//----- -static functions
				     
//-------------------------------------------------
//----- external functions


// initialze renderer
void render_init(void) {
  u32 i;

  for(i = 0; i<numRegions; i++) {
    region_alloc((region*)(allRegions[i]));
  }

  region_alloc(&bootRegion);

  //  screen_clear();

  /* // test */
  /* print_dbg("\r\n\r\n regions:"); */
  /* for(i = 0; i<numRegions; i++) { */
  /*   print_dbg("\r\n ( "); */
  /*   print_dbg_hex(i); */
  /*   print_dbg(" ) @ 0x"); */
  /*   print_dbg_hex((u32)(allRegions[i])); */
  /*   print_dbg(", data: @ 0x"); */
  /*   print_dbg_hex((u32)(allRegions[i]->data)); */
  /*   print_dbg(", w:"); */
  /*   print_dbg_ulong((u32)(allRegions[i]->w)); */
  /*   print_dbg(", h:"); */
  /*   print_dbg_ulong((u32)(allRegions[i]->h)); */
  /*   print_dbg(", len: 0x"); */
  /*   print_dbg_hex((u32)(allRegions[i]->len)); */
  // }
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
  //u32 i, j;

  region_fill(&bigtop, 0x5);
  region_string_aa(&bigtop, "_LPPR", 40, 12, 1);
  region_string(&(foot[0]), "TAP1", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[1]), "TAP2", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[2]), "REC", 0, 0, 0xf, 0x0, 0);
  region_string(&(foot[3]), "PLAY", 0, 0, 0xf, 0x0, 0);

  
  /// test the AA font
  /* for(i=0; i<24; i++) { */
  /*   for(j=0; j<18; j++) { */
  /*     print_dbg_char(0x20 + FONT_AA[3].glyph.data[i * 18 + j]); */
  /*   } */
  /*   print_dbg("\r\n"); */
  /* } */

}

// render text to scrolling buffer during boot procedure
extern void render_boot(const char* str) {
  /// clear current line
  region_fill_part(&bootRegion, 0x0, bootByteOffset, bootRowBytes);
  // draw text to region at current offset, using system font
  region_string(&bootRegion, str,
		0, bootRowOffset * FONT_CHARH, 0xf, 0, 0);
  // advance offset by count of pixels in row
  //  bootOffset += (SCREEN_ROW_PX * FONT_CHARH);
  bootByteOffset += bootRowBytes;
  bootRowOffset++;
  if(bootRowOffset == bootRowCount) {
    bootRowOffset = 0;
  }
  if(bootByteOffset > bootMaxByteOffset) {
    bootByteOffset = 0;
  }

  if(bootCount < bootRowCount) {
    // first 8 items: render from the top 
    bootCount++;
    screen_draw_region(0, 0, 128, 64, bootRegion.data);
  } else {
    // scrolling:
    // render from new ofset with wrap
    screen_draw_region_offset(0, 0, 128, 64, 8192,  
			      bootRegion.data, bootByteOffset);
  }
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
// highlight footer according to SW press
  region* reg;
  switch(sw) {
    // fn switches
  case 0:
    reg = &(foot[0]);
    break;
  case 1:
    reg = &(foot[1]);
    break;
  case 2:
    reg = &(foot[2]);
    break;
  case 3:
    reg = &(foot[3]);
    break;
    // footswitches, same mapping as fn 3,4
  case 6:
    reg = &(foot[2]);
    break;
  case 7:
    reg = &(foot[3]);
    break;
  default:
    return;
    break;
  }
  region_hl(reg, on << 2, 0x8);
}

// draw delay time
void render_delay_time(u8 id, u32 ms, u32 samps) {
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  //  itoa_whole(ms, numstrbuf, 12);
  itoa_whole_lj(ms, numstrbuf);
  region_string_aa(&bigtop, numstrbuf, 30, 18, 1);
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  //  itoa_whole(samps, numstrbuf, 12);
  itoa_whole_lj(samps, numstrbuf);
  region_string_aa(&bigtop, numstrbuf, 30, 36, 1);
}
    
// draw labels for delay time
void render_touched_delaytime(u8 id) {
  region_fill(&bigtop, 0);
  region_string(&bigtop, "delay time", 0, 0, 0xa, 0x3, 0 );
  if(id) { region_string(&bigtop, " 2   ", 40, 0, 0xa, 0x3, 1 ); }
  else { region_string(&bigtop, " 1   ", 40, 0, 0xa, 0x3, 1 ); }
  region_string(&bigtop, "ms ", 	0, 18, 0xd, 0x0, 0 );
  region_string(&bigtop, "samps ", 	0, 36, 0xd, 0x0, 0 );
}

// draw any amplitude
void render_amp(u32 input, s32 amp) {
  // decibels
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  print_fix16(numstrbuf, input_db(input));
  region_string_aa(&bigtop, numstrbuf, 18, 18, 1);
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  print_fix16(numstrbuf, FRACT_FIX16(amp));
  region_string_aa(&bigtop, numstrbuf, 18, 36, 1); 
}

// feedback touched
void render_touched_fb(u8 id) {
  region_fill(&bigtop, 0);
  region_string(&bigtop, "feedback", 0, 0, 0xa, 0x3, 0 );
  if(id) { region_string(&bigtop, " 2   ", 40, 0, 0xa, 0x3, 1 ); }
  else { region_string(&bigtop, " 1   ", 40, 0, 0xa, 0x3, 1 ); }
  region_string(&bigtop, "dB  ", 	0, 18, 0xd, 0x0, 0 );
  region_string(&bigtop, "amp  ", 	0, 36, 0xd, 0x0, 0 );
}

// mix touched
extern void render_touched_mix(u8 id) {
  region_fill(&bigtop, 0);
  region_string(&bigtop, "filter mix", 0, 0, 0xa, 0x3, 0 );
  if(id) { region_string(&bigtop, " 2   ", 40, 0, 0xa, 0x3, 1 ); }
  else { region_string(&bigtop, " 1   ", 40, 0, 0xa, 0x3, 1 ); }
  region_string(&bigtop, "dB  ", 	0, 18, 0xd, 0x0, 0 );
  region_string(&bigtop, "amp  ", 	0, 36, 0xd, 0x0, 0 );
}


// draw frequency
void render_freq(u32 input) {
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  print_fix16(numstrbuf, input_hz(input));
  region_string_aa(&bigtop, numstrbuf, 18, 18, 1);
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  print_fix16(numstrbuf, input_note(input));
  region_string_aa(&bigtop, numstrbuf, 18, 36, 1); 
}

// freq touched
extern void render_touched_freq(u8 id) {
  region_fill(&bigtop, 0);
  region_string(&bigtop, "filter cutoff", 0, 0, 0xa, 0x3, 0 );
  if(id) { region_string(&bigtop, " 2   ", 40, 0, 0xa, 0x3, 1 ); }
  else { region_string(&bigtop, " 1   ", 40, 0, 0xa, 0x3, 1 ); }
  region_string(&bigtop, "hz ", 	0, 18, 0xd, 0x0, 0 );
  region_string(&bigtop, "note ", 	0, 36, 0xd, 0x0, 0 );
}

// draw resonance
void render_res(u32 in) {
  memset(numstrbuf, ' ', NUMSTRBUF_LEN);
  print_fix16(numstrbuf, IN_FR16(in));
  region_string_aa(&bigtop, numstrbuf, 30, 18, 1);
}

// resonance touched
extern void render_touched_res(u8 id) {
  region_fill(&bigtop, 0);
  region_string(&bigtop, "filter RQ", 0, 0, 0xa, 0x3, 0 );
  if(id) { region_string(&bigtop, " 2   ", 40, 0, 0xa, 0x3, 1 ); }
  else { region_string(&bigtop, " 1   ", 40, 0, 0xa, 0x3, 1 ); }
  //  region_string(&bigtop, "[ 0 , 1 ]", 	0, 18, 0xd, 0x0, 0 );
}

extern void render_record(void) {
  region_fill(&bigtop, 0x0);
  region_string_aa(&bigtop, "RECORD", 10, 10, 0);
}


extern void render_overdub(void) {
  region_fill(&bigtop, 0x0);
  region_string_aa(&bigtop, "OVERDUB", 10, 10, 0);
}

extern void render_play(void) {
  region_fill(&bigtop, 0x0);
  region_string_aa(&bigtop, "PLAY", 10, 10, 0);
}


void render_test(const char* str) {
  region_fill(&bigtop, 0);
  region_string_aa(&bigtop, str, 0, 0, 0 );
  region_string_aa(&bigtop, str, 0, 20, 1 );
}

#undef LINE_BUF_LEN
