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

// all function keys take a double-press
static u8 keyPressed = 255;

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
    clearln();
    appendln_idx_lj(opIdx);
    appendln_char('.');
    appendln( net_op_name(opIdx) );
    appendln_char('/');
    appendln( net_in_name(idx) );
    endln();

    font_string_region_clip(tmpRegion, lineBuf, 0, 0, 0xa, 0);
    clearln();

    print_fix16(lineBuf, net_get_in_value(idx));
    font_string_region_clip(tmpRegion, lineBuf, LINE_VAL_POS, 0, 0xa, 0);
  } else {
    // parameter input    
    clearln();
    appendln_idx_lj( (int)net_param_idx(idx)); 
    endln();
    appendln( net_in_name(idx)); 
    endln();
    font_string_region_clip(tmpRegion, lineBuf, 0, 0, 0xa, 0);
    clearln();
    print_fix16(lineBuf, net_get_in_value(idx));
    font_string_region_clip(tmpRegion, lineBuf, 48, 0, 0xa, 0);
  }
  // underline
  region_fill_part(tmpRegion, LINE_UNDERLINE_OFFSET, LINE_UNDERLINE_LEN, 0x1);
}

// edit the current seleciton
static void select_edit(s32 inc) {
  // increment input value
  net_inc_in_value(curPage->select, inc);
  // render to tmp buffer
  render_line(curPage->select);
  // copy to scroll with highlight
  render_to_scroll_line(SCROLL_CENTER_LINE, 1);
}

// scroll the current selection
static void select_scroll(s32 dir) {
  const s32 max = net_num_ins() - 1 + net_num_params();
  // index for new content
  s16 newIdx;
  s16 newSel;

  if(dir < 0) {
    /// SCROLL DOWN
    // if selection is already zero, do nothing 
    if(curPage->select == 0) {
      //      print_dbg("\r\n reached min selection in inputs scroll. ");
      return;
    }
    // remove highlight from old center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);
    // decrement selection
    newSel = curPage->select - 1;
    ///// these bounds checks shouldn't really be needed here...
    //    if(newSel < 0) { newSel = 0; }
    //    if(newSel > max ) { newSel = max; }
    curPage->select = newSel;    
    // add new content at top
    newIdx = newSel - SCROLL_LINES_BELOW;
    if(newIdx < 0) { 
      // empty row
      region_fill(tmpRegion, 0);
    } else {
      render_line(newIdx);
    }
    // render tmp region to bottom of scroll
    // (this also updates scroll byte offset) 
    render_to_scroll_top();
    // add highlight to new center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);

  } else {
    // SCROLL UP
    // if selection is already max, do nothing 
    if(curPage->select == max) {
      //      print_dbg("\r\n reached max selection in inputs scroll. ");
      return;
    }
    // remove highlight from old center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);
    // increment selection
    newSel = curPage->select + 1;
    ///// these bounds checks shouldn't really be needed here...
    //    if(newSel < 0) { newSel = 0; }
    //    if(newSel > max ) { newSel = max; }
    /////
    curPage->select = newSel;    
    // add new content at bottom of screen
    newIdx = newSel + SCROLL_LINES_ABOVE;
    if(newIdx > max) { 
      // empty row
      region_fill(tmpRegion, 0);
    } else {
      render_line(newIdx);
    }
    // render tmp region to bottom of scroll
    // (this also updates scroll byte offset) 
    render_to_scroll_bottom();
    // add highlight to new center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);
  }
}

// display the function key labels according to current state
/// FIXME: would be more maintainable to use enum and array of funtcions. i guess
static void show_foot0(void) {
  u8 fill = 0;
  if(keyPressed == 0) {
    fill = 0x5;
  }
  region_fill(footRegion[0], fill);
  if(altMode) {
    font_string_region_clip(footRegion[0], "GATHER", 0, 0, 0xf, fill);
  } else {
    font_string_region_clip(footRegion[0], "STORE", 0, 0, 0xf, fill);
  }
}

static void show_foot1(void) {
  u8 fill = 0;
  if(keyPressed == 1) {
    fill = 0x5;
  }
  region_fill(footRegion[1], fill);
  if(altMode) {
    if(inPlay) {
      font_string_region_clip(footRegion[1], "HIDE", 0, 0, 0xf, fill);
    } else {
      font_string_region_clip(footRegion[1], "SHOW", 0, 0, 0xf, fill);
    }
  } else {
    if(inPreset) {
      font_string_region_clip(footRegion[1], "EXC", 0, 0, 0xf, fill);
    } else {
      font_string_region_clip(footRegion[1], "INC", 0, 0, 0xf, fill);
    }
  }
}

static void show_foot2(void) {
  u8 fill = 0;
  if(keyPressed == 2) {
    fill = 0x5;
  }
  region_fill(footRegion[2], fill);
  if(altMode) {
    if(playFilter) {
      font_string_region_clip(footRegion[2], "ALL", 0, 0, 0xf, fill);
    } else {
      font_string_region_clip(footRegion[2], "FILT", 0, 0, 0xf, fill);
    }
  } else {
    font_string_region_clip(footRegion[2], "CLEAR", 0, 0, 0xf, fill);
  }
}

static void show_foot3(void) {
  u8 fill = 0;
  u8 fore = 0xf;
  if(altMode) {
    fill = 0xf;
    fore = 0;
  }
  region_fill(footRegion[3], fill);
  font_string_region_clip(footRegion[3], "ALT", 0, 0, fore, fill);
}


static void show_foot(void) {
  if(clearConfirm) {
    font_string_region_clip(footRegion[0], "-   ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[1], "-   ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[2], "-   ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[3], " OK! ", 0, 0, 0xf, 0x5);
  } else { 
    show_foot0();
    show_foot1();
    show_foot2();
    show_foot3();
  }
  /*
/// FIXME:
    // it would be more efficient (redundant compares, etc) 
    // to combine the fn's above into an ugly if-nest as below.
    // we don't ever really update the regions independently.

  u8 i;
  u8 fill[4] = {0, 0, 0, 0};
  static const u8 footpx[4] = {0, 32, 64, 96};
  for(i = 0; i<4; i++) {
    if(keyPressed == i) { fill[i] = 0x5; }
    //    region_fill_part(footRegion, footpx[i], 32, fill[i]);
  }
  if(clearConfirm) {
    font_string_region_clip(footRegion, "-   ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion, "-   ", 32, 0, 0xf, 0);
    font_string_region_clip(footRegion, "-   ", 64, 0, 0xf, 0);
    font_string_region_clip(footRegion, " OK! ", 96, 0, 0xf, 0x5);
  } else {
    if(altMode) {
      ///// ALT
      font_string_region_clip(footRegion, " GATHR ", 0, 0, 0xf, fill[0]);
      if(inPlay) {
	font_string_region_clip(footRegion, " HIDE ", 32, 0, 0xf, fill[1]);
      } else {
	font_string_region_clip(footRegion, " SHOW ", 32, 0, 0xf, fill[1]);
      }
      if(playFilter) {
	font_string_region_clip(footRegion, " ALL  ", 64, 0, 0xf, fill[3]);
      } else {
	font_string_region_clip(footRegion, " FILT ", 64, 0, 0xf, fill[3]);
      }
      font_string_region_clip(footRegion, " ALT ", 96, 0, 0x0, 0xf);
    } else {
      ///// non-ALT
      font_string_region_clip(footRegion, " STORE ", 0, 0, 0xf, fill[0]);
      if(inPreset) {
	font_string_region_clip(footRegion, " EXC  ", 32, 0, 0xf, fill[1]);
      } else {
	font_string_region_clip(footRegion, " INC  ", 32, 0, 0xf, fill[1]);
      }
      font_string_region_clip(footRegion, " CLEAR  ", 64, 0, 0xf, fill[2]);
      font_string_region_clip(footRegion, " ALT ", 96, 0, 0xf, fill[3]);
    }
  }
  */
}

// set alt mode
static void set_alt(u8 val) {
  altMode = val;
  show_foot();
}

//----------------------
// ---- extern 
// init
void init_page_ins(void) {
  u8 i, n;
  print_dbg("\r\n alloc INS page");
  // allocate regions
  region_alloc(&scrollRegion);
  // fill regions
  region_fill(&scrollRegion, 0x0);
  // fill the scroll with actual line values...
  n = 3;
  i = 0;
  //// need to actually set the scroll region at least temporarily
  render_set_scroll_region(&scrollRegion);
  while(i<5) {
    render_line(i);
    render_to_scroll_line(n, i == 0 ? 1 : 0);
    ++n;
    ++i;
  }
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
  font_string_region_clip(headRegion, "INPUTS", 0, 0, 0xf, 0x1);
}

//--------------------------
//---- static definitions

static u8 check_key(u8 key) {
  u8 ret;
  if(keyPressed == key) {
    keyPressed = 255;
    ret = 1;
  } else {
    keyPressed = key;
    ret = 0;
  }
  return ret;
}

// function keys
void handle_key_0(s32 val) {
  if(val == 0) { return; }
  if(check_key(0)) {
    if(altMode) {
      // gather
    } else {
      // store in preset (+ scene?)
    }
  }
  show_foot();
}

void handle_key_1(s32 val) {
  //  print_dbg("\r\n page_ins handle key 1 ");
  if(val == 0) { return; }
  if(check_key(1)) {
    if(altMode) {
      // include / exclude
    } else {
      // show / hide
    }
  }
  show_foot();
}

void handle_key_2(s32 val) {
  //  print_dbg("\r\n page_ins handle key 2 ");
  if(val == 0) { return; }
  if(check_key(2)) {
    if(altMode) {
      // filter / all
    } else {
      // clear (disconnect all routings) / CONFIRM
    }
  } 
  show_foot();
}

void handle_key_3(s32 val) {
  //  print_dbg("\r\n page_ins handle key 3 ");
  // alt mode
  set_alt((u8)val);
}

void handle_enc_0(s32 val) {
  //  print_dbg("\r\n page_ins handle enc 0 ");
  // change parameter value, slow
  select_edit(scale_knob_value(val));
}

void handle_enc_1(s32 val) {
  //  print_dbg("\r\n page_ins handle enc 1 ");
  // change parameter value, fast
  select_edit(scale_knob_value(val) << 16);
}

void handle_enc_2(s32 val) {
  //  print_dbg("\r\n page_ins handle enc 2 ");
  // scroll page
  if(val > 0) {
    //    set_page(ePageOuts);
  } else {
    //    set_page(ePagePresets);
  }
}

void handle_enc_3(s32 val) {
  //  print_dbg("\r\n page_ins handle enc 3 ");
  // scroll selection
  select_scroll(val);
}
