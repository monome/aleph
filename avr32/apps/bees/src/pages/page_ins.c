/*
  page_ins.c
 */

// std
// #include <string.h>
// #include <stdio.h>

// asf
#include "print_funcs.h"

// common
#include "fix.h"

// avr32
#include "region.h"

// bees
#include "handler.h"
#include "menu_protected.h"
#include "pages.h"
#include "print_funcs.h"
#include "render.h"

//-------------------------
//---- static variables

static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };

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

//-------------------------
//---- static declarations

// handlers
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// array of handlers
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

// fill tmp region with new content
// given input index
static void render_line(s16 idx) {
  const s16 opIdx = net_in_op_idx(idx);
  region_fill(tmpRegion, 0x0);
  if(opIdx >= 0) {
    // operator input
    // build descriptor string
    println_int(opIdx, 0);
    appendln_char(':');
    println( net_op_name(opIdx), 0 );
    appendln_char('/');
    appendln( net_in_name(idx) );
    endln();

    /* print_dbg("\r\n line buffer: "); */
    /* print_dbg(lineBuf); */

    /* print_dbg("\r\n rendering to tmp buffer, dst : 0x (not really)"); */
    /* print_dbg_hex((u32)(tmpRegion->data)); */

    region_string(tmpRegion, lineBuf, 0, 0, 0xf, 0, 0);
    clearln();

    /* print_dbg("\r\n printing input value to line buf... idx: "); */
    /* print_dbg_ulong(idx); */
    /* print_dbg(" , value: 0x"); */
    /* print_dbg_hex(net_get_in_value(idx)); */

    print_fix16(lineBuf, net_get_in_value(idx));

    /* print_dbg("\r\n line buffer: "); */
    /* print_dbg(lineBuf); */

    region_string(tmpRegion, lineBuf, 80, 0, 0xf, 0, 0);
  } else {
    // parameter input    
    println_int( (int)net_param_idx(idx), 0); endln();
    println( net_in_name(idx), 0); endln();

    /* print_dbg("\r\n line buffer: "); */
    /* print_dbg(lineBuf); */

    region_string(tmpRegion, lineBuf, 0, 0, 0xf, 0, 0);
    clearln();
    print_fix16(lineBuf, net_get_in_value(idx));

    /* print_dbg("\r\n line buffer: "); */
    /* print_dbg(lineBuf); */

    region_string(tmpRegion, lineBuf, 0, 0, 0xf, 0, 0);
  }
}

// edit the current seleciton
static void select_edit(s32 inc) {
  /* print_dbg("\r\n handle vlaue knob, selection: "); */
  /* print_dbg_ulong(curPage->select); */
  /* print_dbg(", value: 0x"); */
  /* print_dbg_ulong(inc); */

  // increment input value
  //  print_dbg("\r\n incrementing input value...");
  net_inc_in_value(curPage->select, inc);

  // render to tmp buffer
  //  print_dbg("\r\n rendering to tmp...");
  render_line(curPage->select);

  // copy to scroll with clipping
  //  print_dbg("\r\n copying to scroll center...");
  render_to_scroll_center();

  // copy to selection with highlight
  //  print_dbg("\r\n copying to selection...");
  render_to_select();
}

// scroll the current selection
static void select_scroll(s8 dir) {
  // index for new content
  s16 newIdx;
  s16 newSel;
  // write new content to scroll (at beginning or end)
  /// direction is binary, forward or backward
  if(dir < 0) {
    // decrement selection
    newSel = pages[ePageIns].select - 1;
    if(newSel < 0) { newSel = 0; }
    if(newSel > (net_num_ins()-1) ) { newSel = net_num_ins() - 1; }
    pages[ePageIns].select = newSel;    

    /* print_dbg("\r\n new selection index: "); */
    /* print_dbg_ulong(newSel); */

    // new content is (selection - center row) 
    newIdx = newSel + SCROLL_LINES_ABOVE_1;

    // render new content
    render_line(newIdx);

    // copy to top of scroll
    //    print_dbg("\r\n rendering temp to scroll top...");
    render_to_scroll_top();

  } else {
    // incerement selection
    newSel = pages[ePageIns].select + 1;
    if(newSel < 0) { newSel = 0; }
    if(newSel > (net_num_ins()-1) ) { newSel = net_num_ins() - 1; }
    pages[ePageIns].select = newSel;    
    
    /* print_dbg("\r\n new selection index: "); */
    /* print_dbg_ulong(newSel); */

    // new content is )(selection + (num rows - center row))
    newIdx = pages[ePageIns].select + SCROLL_LINES_ABOVE_1;
    // render new content
    render_line(newIdx);

    // copy to bottom of scrol
    //    print_dbg("\r\n rendering temp to scroll bottom...");
    render_to_scroll_bottom();
  //  render_to_select();
  }
  // copy new center region
  //  print_dbg("\r\n copying scroll center to selection region");
  render_from_scroll_center();
}

// display the function key labels according to current state
static void show_foot(void) {
  region_fill(footRegion, 0x0);
  if(clearConfirm) {
    region_string(footRegion, "-   ", 0, 0, 0xf, 0x1, 0);
    region_string(footRegion, "-   ", 32, 0, 0xf, 0x1, 0);
    region_string(footRegion, "-   ", 64, 0, 0xf, 0x1, 0);
    region_string(footRegion, "OK! ", 96, 0, 0xf, 0x1, 0);
  } else {
    if(altMode) {
      region_string(footRegion, "GATHR", 0, 0, 0xf, 0x1, 0);
      if(inPlay) {
	region_string(footRegion, "HIDE", 32, 0, 0xf, 0x1, 0);
      } else {
	region_string(footRegion, "SHOW", 32, 0, 0xf, 0x1, 0);
      }
      if(playFilter) {
	region_string(footRegion, "ALL", 64, 0, 0xf, 0x1, 0);
      } else {
	region_string(footRegion, "FILT", 64, 0, 0xf, 0x1, 0);
      }
      region_string(footRegion, "ALT", 96, 0, 0xf, 0x5, 0);
    } else {
      region_string(footRegion, "STORE", 0, 0, 0xf, 0x1, 0);
      if(inPreset) {
	region_string(footRegion, "EXC", 32, 0, 0xf, 0x1, 0);
      } else {
	region_string(footRegion, "INC", 32, 0, 0xf, 0x1, 0);
      }
      region_string(footRegion, "CLEAR", 64, 0, 0xf, 0x1, 0);
      region_string(footRegion, "ALT", 96, 0, 0xf, 0x1, 0x0);
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
  print_dbg("\r\n alloc INS page");
  // allocate regions
  region_alloc(&scrollRegion);

  //  print_dbg("\r\n filling scroll region... ");
  //  print_dbg(" ( not really ) "); 
  // fill regions
  region_fill(&scrollRegion, 0x0);
}

// refresh
// called when this page is selected
void refresh_ins(void) { 
  print_dbg("\r\n refresh INS... ");
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll_region(&scrollRegion);

  
  
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  region_string(headRegion, "INPUTS", 0, 0, 0xf, 0x1, 0);
}

//--------------------------
//---- static definitions

// function keys
void handle_key_0(s32 val) {

  print_dbg("\r\n page_ins handle key 0 ");
  // test
  net_print();

  if(altMode) {
    // gather
  } else {
    // store in preset (+ scene?)
  }
}

void handle_key_1(s32 val) {

  print_dbg("\r\n page_ins handle key 1 ");
  if(altMode) {
    // include / exclude
  } else {
    // show / hide
  }
}

void handle_key_2(s32 val) {
  print_dbg("\r\n page_ins handle key 2 ");
  if(altMode) {
    // filter / all
  } else {
    // clear (disconnect all routings) / CONFIRM
  }
}

void handle_key_3(s32 val) {
  print_dbg("\r\n page_ins handle key 3 ");
  // alt mode
  set_alt((u8)val);
}

void handle_enc_0(s32 val) {
  print_dbg("\r\n page_ins handle enc 0 ");
  
  // change parameter value, slow
  select_edit(scale_knob_value(val));
}

void handle_enc_1(s32 val) {
  print_dbg("\r\n page_ins handle enc 1 ");
  // change parameter value, fast
}

void handle_enc_2(s32 val) {
  print_dbg("\r\n page_ins handle enc 2 ");
  // scroll page
}

void handle_enc_3(s32 val) {
  print_dbg("\r\n page_ins handle enc 3 ");
  // scroll selection
  select_scroll(val);
}
