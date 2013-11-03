/*
  page_outs.c
 */

// asf
#include "print_funcs.h"

// bees
#include "handler.h"
#include "net.h"
#include "pages.h"
#include "render.h"

//====================================
//==== static variables

//--- these are common to all pages
// a region for the center scroll
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };
// a scroll class that manages write/read offsets into the scroll region
static scroll centerScroll;

//--- page-specific state variables
// selection-included-in-preset flag (read from network on selection)
static u8 inPreset = 0;
// confirm clear operation
static u8 clearConfirm = 0;

//==========================================
//===== static function declarations

// handler declarations
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// show footers
static void show_foot0(void);
static void show_foot1(void);
static void show_foot2(void);
static void show_foot3(void);
static void show_foot(void);

/* // array of handlers */
/* const page_handler_t handler_outs[eNumPageHandlers] = { */
/*   &handle_enc_0, */
/*   &handle_enc_1, */
/*   &handle_enc_2, */
/*   &handle_enc_3, */
/*   &handle_key_0, */
/*   &handle_key_1, */
/*   &handle_key_2, */
/*   &handle_key_3, */
/* }; */

// fill tmp region with new content
// given input index
static void render_line(s16 idx) {
  //  const s16 opIdx = net_in_op_idx(idx);
  s16 target;
  s16 targetOpIdx;
  region_fill(lineRegion, 0x0);

  target = net_get_target(idx);

  if(target >= 0) {
    //// output has target
    // render output
    clearln();
    appendln_idx_lj(net_out_op_idx(idx));
    appendln_char('.');
    appendln( net_op_name(net_out_op_idx(idx)));
    appendln_char('/');
    appendln( net_out_name(idx) );
    endln();
    font_string_region_clip(lineRegion, lineBuf, 0, 0, 0xa, 0);
    // render target
    targetOpIdx = net_in_op_idx(target);
    clearln();
    appendln("-> ");
    if(targetOpIdx >= 0) {
      // target is operator input
      appendln_idx_lj(net_in_op_idx(target));
      appendln_char('.');
      appendln( net_op_name(net_in_op_idx(target)) );
      appendln_char('/');
      appendln( net_in_name(target) );
    } else {
      // target is parameter input
      appendln_idx_lj( (int)net_param_idx(target)); 
      appendln_char('.');
      appendln( net_in_name(target)); 
    }
    endln();
    font_string_region_clip(lineRegion, lineBuf, 60, 0, 0xa, 0);
    clearln();
    //    print_fix16(lineBuf, net_get_in_value(idx));
    //    font_string_region_clip(lineRegion, lineBuf, LINE_VAL_POS, 0, 0xa, 0);

  } else {
    //// no target
    // render output
    clearln();
    appendln_idx_lj(net_out_op_idx(idx));
    appendln_char('.');
    appendln( net_op_name(net_out_op_idx(idx)));
    appendln_char('/');
    appendln( net_out_name(idx) );
    endln();
    font_string_region_clip(lineRegion, lineBuf, 0, 0, 0xa, 0);
  }
  // underline
  region_fill_part(lineRegion, LINE_UNDERLINE_OFFSET, LINE_UNDERLINE_LEN, 0x1);
}

// edit the current seleciton
static void select_edit(s32 inc) {
  s16 target = net_get_target(curPage->select);
  if(inc > 0) {
    // increment target
    ++target;
    if(target == net_num_ins()) {
      // scroll past all inputs : disconnect and wrap
      target = -1;
    }
  } else {
    --target;
    if (target == -2) {
      //  scrolled down from disconnect: connect and wrap
      target = net_num_ins() - 1;
    }
  }
  net_connect(curPage->select, target);
  // render to tmp buffer
  render_line(curPage->select);
  // copy to scroll with highlight
  render_to_scroll_line(SCROLL_CENTER_LINE, 1);
}

// scroll the current selection
static void select_scroll(s32 dir) {
  const s32 max = net_num_outs() - 1;
  // index for new content
  s16 newIdx;
  s16 newSel;
  // new flags
  u8 newInPreset;

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
    // update preset-inclusion flag
    inPreset = (u8)net_get_out_preset((u32)(curPage->select));
    // add new content at top
    newIdx = newSel - SCROLL_LINES_BELOW;
    if(newIdx < 0) { 
      // empty row
      region_fill(lineRegion, 0);
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
    // update preset-inclusion flag
    inPreset = (u8)net_get_out_preset((u32)(curPage->select));
    // add new content at bottom of screen
    newIdx = newSel + SCROLL_LINES_ABOVE;
    if(newIdx > max) { 
      // empty row
      region_fill(lineRegion, 0);
    } else {
      render_line(newIdx);
    }
    // render tmp region to bottom of scroll
    // (this also updates scroll byte offset) 
    render_to_scroll_bottom();
    // add highlight to new center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);
  }
  /// update flags
  newInPreset = net_get_in_preset(curPage->select);
  if(newInPreset != inPreset) {
    inPreset = newInPreset;
    // update inc/exc label
    show_foot1();
  }
}

// display the function key labels according to current state
static void show_foot0(void) {
  u8 fill = 0;
  if(keyPressed == 0) {
    fill = 0x5;
  }
  region_fill(footRegion[0], fill);
  if(altMode) {
    font_string_region_clip(footRegion[0], "FOLLOW", 0, 0, 0xf, fill);
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
    font_string_region_clip(footRegion[1], "SPLIT", 0, 0, 0xf, fill);
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
  font_string_region_clip(footRegion[2], "CLEAR", 0, 0, 0xf, fill);
  
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
    font_string_region_clip(footRegion[0], "-    ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[1], "-    ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[2], "OK!  ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[3], "-    ", 0, 0, 0xf, 0x5);
  } else { 
    /// FIXME: each of these fn's compares altMode flag. dumb
    show_foot0();
    show_foot1();
    show_foot2();
    show_foot3();
  }
  
}

//----------------------
// ---- extern

// init
void init_page_outs(void) {
  u8 i, n;
  print_dbg("\r\n alloc OUTS page");
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
    render_line(i);
    render_to_scroll_line(n, i == 0 ? 1 : 0);
    ++n;
    ++i;
  }
}
 
// select 
void select_outs(void) {
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "OUTPUTS", 0, 0, 0xf, 0x1);
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

// function key handlers
void handle_key_0(s32 val) {
  // store (follow)
}

void handle_key_1(s32 val) {
  // inc/exc (split)
}

void handle_key_2(s32 val) {
  // clear/confirm
}

void handle_key_3(s32 val) {
  // alt mode
  altMode = (u8)(val > 0);
  show_foot();
}

// encoder handlers
void handle_enc_0(s32 val) { 
  // edit selection (target)
  select_edit(val);
}

void handle_enc_1(s32 val) {
}

void handle_enc_2(s32 val) {
  // scroll page
  if(val > 0) {
    set_page(ePageDsp);
  } else {
    set_page(ePageIns);
  }
}

void handle_enc_3(s32 val) {
  // scroll selection
  select_scroll(val);
}

