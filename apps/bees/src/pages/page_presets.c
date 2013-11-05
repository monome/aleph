/*
  page_presets.c
 */

// asf
#include "print_funcs.h"

// bees
#include "handler.h"
#include "net.h"
#include "pages.h"
#include "render.h"


//--------------------------
//----- static variables

//--- these are common to all pages
// a region for the center scroll
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };
// a scroll class that manages write/read offsets into the scroll region
static scroll centerScroll;

//--- page-specific state variables
// in copy operation
static u8 inCopy = 0;

//-------------------------
//---- static declarations

// handler declarations
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

static void show_foot(void);
static void show_foot0(void);
static void show_foot1(void);
static void show_foot2(void);
static void show_foot3(void);

// render line given index, foreground color
static void render_line(s16 idx, u8 fg);
// scroll current selection
static void select_scroll(s32 dir);

//-----------------------------
//------ static definition

void render_line(s16 idx, u8 fg) {
  clearln();
  appendln_idx_lj(idx);
  
}
// scroll current selection
void select_scroll(s32 dir) {
}


// display the function key labels according to current state
 void show_foot0(void) {
  u8 fill = 0;
  if(keyPressed == 0) {
    fill = 0x5;
  }
  font_string_region_clip(footRegion[0], "STORE", 0, 0, 0xf, fill);
}

 void show_foot1(void) {
  u8 fill = 0;
  if(keyPressed == 1) {
    fill = 0x5;
  }
  region_fill(footRegion[1], fill);
  font_string_region_clip(footRegion[1], "RECALL", 0, 0, 0xf, fill);
}

 void show_foot2(void) {
  u8 fill = 0;
  if(keyPressed == 2) {
    fill = 0x5;
  }
  region_fill(footRegion[2], fill);
  font_string_region_clip(footRegion[2], "COPY", 0, 0, 0xf, fill); 
}

 void show_foot3(void) {
  u8 fill = 0;
  u8 fore = 0xf;
  region_fill(footRegion[3], fill);
  font_string_region_clip(footRegion[3], "DEFAULT", 0, 0, fore, fill);
}


 void show_foot(void) {
  if(inCopy) {
    font_string_region_clip(footRegion[0], "-    ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[1], "-    ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[2], "OK!  ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[3], "-    ", 0, 0, 0xf, 0x5);
  } else { 
    show_foot0();
    show_foot1();
    show_foot2();
    show_foot3();
  } 
}


// function keys
void handle_key_0(s32 val) {
  // store selected
  show_foot();
}

void handle_key_1(s32 val) {
  // recall selected
  show_foot();
}

void handle_key_2(s32 val) {
  // copy / confirm
  show_foot();
}

void handle_key_3(s32 val) {
  // default
  show_foot();
}

void handle_enc_0(s32 val) {
  // scroll position in name
}

void handle_enc_1(s32 val) {
  // scroll character at position in name
}

void handle_enc_2(s32 val) {
  // scroll page
  if(val > 0) {
    set_page(ePageDsp);
  } else {
    set_page(ePageOuts);
  }
}

void handle_enc_3(s32 val) {
  // scroll selection
  select_scroll(val);
}


//----------------------
// ---- extern 

// init
void init_page_presets(void) {
  u8 i, n;
  print_dbg("\r\n alloc PRESETS page");
  // allocate regions
  region_alloc(&scrollRegion);
  // init scroll
  scroll_init(&centerScroll, &scrollRegion);
  // fill regions
  region_fill(&scrollRegion, 0x0);
  // fill the scroll with actual line values...
  n = 3;
  i = 0;
  //// need to actually set the scroll region at least temporarily
  render_set_scroll(&centerScroll);
  while(i<5) {
    render_line(i, 0xa);
    render_to_scroll_line(n, i == 0 ? 1 : 0);
    ++n;
    ++i;
  }
}

// select
void select_presets(void) {
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "PRESETS", 0, 0, 0xf, 0x1);
  // assign handlers
  app_event_handlers[ kEventEncoder0 ]	= &handle_enc_0 ;
  app_event_handlers[ kEventEncoder1 ]	= &handle_enc_1 ;
  app_event_handlers[ kEventEncoder2 ]	= &handle_enc_2 ;
  app_event_handlers[ kEventEncoder3 ]	= &handle_enc_3 ;
  app_event_handlers[ kEventSwitch0 ]	= &handle_key_0 ;
  app_event_handlers[ kEventSwitch1 ]	= &handle_key_1 ;
  app_event_handlers[ kEventSwitch2 ]	= &handle_key_2 ;
  app_event_handlers[ kEventSwitch3 ]	= &handle_key_3 ;
}
