/*
  page_ins.c
 */

// avr32
#include "region.h"

// bees
#include "menu_protected.h"
#include "pages.h"
#include "render.h"

//-------------------------
//---- static variables
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };
static region headRegion = { .w = 128, .h = 8, .x = 0, .y = 0 };
static region footRegion = { .w = 128, .h = 8, .x = 0, .y = 120 };
static region centerRegion = {  .w = 128, .h = 8, .x = 0, .y = 16 };

// alt-mode flag (momentary)
static u8 altMode = 0;
// play-mode filter flag (persistent)
static u8 playFilter = 0;
// selection-included-in-preset flag (read from network on selection)
static u8 inPreset = 0;
// selection-included-in-play flag (read from network on selection)
static u8 inPlay = 0;
// in-clear-confirm state
static u8 clearConfirm = 0;

// scrolling parameters



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


// draw scrolling region
// (presumably just changed selection)
static void scroll_refresh(void) {
  // write old selection-buffer to center of scrollbuf, no highlight
  // change scroll
  // update selection-buffer
  // write new selection buffer to center, with highlight
  // mark scroll region as dirty
}

// display the function key labels according to current state
static void show_foot(void) {
  if(clearConfirm) {
    region_string(&footRegion, "-   ", 0, 0, 0xf, 0x1, 0);
    region_string(&footRegion, "-   ", 32, 0, 0xf, 0x1, 0);
    region_string(&footRegion, "-   ", 64, 0, 0xf, 0x1, 0);
    region_string(&footRegion, "OK! ", 96, 0, 0xf, 0x1, 0);
  } else {
    if(altMode) {
      region_string(&footRegion, "GATHR", 0, 0, 0xf, 0x1, 0);
      if(inPlay) {
	region_string(&footRegion, "HIDE", 32, 0, 0xf, 0x1, 0);
      } else {
	region_string(&footRegion, "SHOW", 32, 0, 0xf, 0x1, 0);
      }
      if(playFilter) {
	region_string(&footRegion, "ALL", 64, 0, 0xf, 0x1, 0);
      } else {
	region_string(&footRegion, "FILT", 64, 0, 0xf, 0x1, 0);
      }
      region_string(&footRegion, "ALT", 96, 0, 0xf, 0x5, 0);
    } else {
      region_string(&footRegion, "STORE", 0, 0, 0xf, 0x1, 0);
      if(inPreset) {
	region_string(&footRegion, "EXC", 32, 0, 0xf, 0x1, 0);
      } else {
	region_string(&footRegion, "INC", 32, 0, 0xf, 0x1, 0);
      }
      region_string(&footRegion, "CLEAR", 64, 0, 0xf, 0x1, 0);
      region_string(&footRegion, "ALT", 96, 0, 0xf, 0x1, 0x0);
    }
  }
}

static void set_alt(u8 val) {
  altMode = val;
  show_foot();
}

//----------------------
// ---- extern 
// init
void init_page_ins(void) {
  // allocate regions
  region_alloc(&scrollRegion);
  region_alloc(&headRegion);
  region_alloc(&footRegion);
  region_alloc(&centerRegion);
  // fill regions
  region_fill(&scrollRegion, 0x0);
  region_fill(&headRegion, 0x0);
  region_fill(&footRegion, 0x0);
  // initial strings
  region_string(&headRegion, "INPUTS", 0, 0, 0xf, 0x1, 0);
  region_string(&footRegion, "STORE", 0, 0, 0xf, 0x1, 0);
  region_string(&footRegion, "INC", 0, 0, 0xf, 0x1, 0);
  region_string(&footRegion, "CLEAR", 0, 0, 0xf, 0x1, 0);
  region_string(&footRegion, "ALT", 0, 0, 0xf, 0x1, 0);
}

// refresh
// called when this page is selected
void refresh_ins(void) { 
  // assign global region pointers to this page's regions
  // this also marks them dirty
  render_set_scroll_region(&scrollRegion);
  render_set_head_region(&headRegion);
  render_set_foot_region(&footRegion);
}

const page_handler_t handler_ins[eNumPageHandlers] = {
  &handle_enc_0,
  &handle_enc_1,
  &handle_enc_2,
  &handle_enc_3,
  &handle_key_0,
  &handle_key_1,
  &handle_key_2,
  &handle_key_3,
};


//--------------------------
//---- static definitions

// function keys
void handle_key_0(s32 val) {
  if(altMode) {
    // gather
  } else {
    // store in preset (+ scene?)
  }
}

void handle_key_1(s32 val) {
  if(altMode) {
    // include / exclude
  } else {
    // show / hide
  }
}

void handle_key_2(s32 val) {
  if(altMode) {
    // filter / all
  } else {
    // clear (disconnect all routings) / CONFIRM
  }
}

void handle_key_3(s32 val) {
  // alt mode
  set_alt((u8)val);
}

void handle_enc_0(s32 val) {
  // change parameter value, slow
}

void handle_enc_1(s32 val) {
  // change parameter value, fast
}

void handle_enc_2(s32 val) {
  ;;  // not used
}

void handle_enc_3(s32 val) {
  ;; // not used
}
