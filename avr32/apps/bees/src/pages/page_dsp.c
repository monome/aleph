/*
  page_dsp.c
 */

// asf
#include "print_funcs.h"

// bees
#include "files.h"
#include "handler.h"
#include "pages_protected.h"
#include "pages.h"
#include "render.h"

//-------------------------
//---- static variables

// scroll region
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };
// scroll manager
static scroll centerScroll;

// in-write-confirm state
static u8 writeConfirm = 0;


// handler declarations
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// array of handlers
const page_handler_t handler_dsp[eNumPageHandlers] = {
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
static void render_line(s16 idx, u16 max) {
  //  const s16 opIdx = net_in_op_idx(idx);
  region_fill(lineRegion, 0x0);
  if((idx > max) || (idx < 0)) { return; }
  clearln();
  print_dbg("\r\n render DSP line: ");
  print_dbg(files_get_dsp_name(idx));
 
  appendln((const char*)files_get_dsp_name(idx));
  endln();

  print_dbg("\r\n line buffer: ");
  print_dbg(lineBuf);

  font_string_region_clip(lineRegion, lineBuf, 0, 0, 0xa, 0);
  //  region_fill_part(lineRegion, LINE_UNDERLINE_OFFSET, LINE_UNDERLINE_LEN, 0x1);
}


// scroll the current selection
static void select_scroll(s32 dir) {
  const s32 max = files_get_dsp_count() - 1;
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
    // update preset-inclusion flag
   
    // add new content at top
    newIdx = newSel - SCROLL_LINES_BELOW;
    if(newIdx < 0) { 
      // empty row
      region_fill(lineRegion, 0);
    } else {
      render_line(newIdx, max);
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
    //    inPreset = (u8)net_get_in_preset((u32)(curPage->select));
    
    // add new content at bottom of screen
    newIdx = newSel + SCROLL_LINES_ABOVE;
    if(newIdx > max) { 
      // empty row
      region_fill(lineRegion, 0);
    } else {
      render_line(newIdx, max);
    }
    // render tmp region to bottom of scroll
    // (this also updates scroll byte offset) 
    render_to_scroll_bottom();
    // add highlight to new center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);
  }
}

// display the function key labels according to current state
static void show_foot0(void) {
  u8 fill = 0;
  if(keyPressed == 0) {
    fill = 0x5;
  }
  /*
  region_fill(footRegion[0], fill);
  if(altMode) {
    font_string_region_clip(footRegion[0], "GATHER", 0, 0, 0xf, fill);
  } else {
    font_string_region_clip(footRegion[0], "STORE", 0, 0, 0xf, fill);
  }
  */
}

static void show_foot1(void) {
  u8 fill = 0;
  if(keyPressed == 1) {
    fill = 0x5;
  }
  /*
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
  */
}

static void show_foot2(void) {
  u8 fill = 0;
  if(keyPressed == 2) {
    fill = 0x5;
  }
  region_fill(footRegion[2], fill);
  /*
  if(altMode) {
    if(playFilter) {
      font_string_region_clip(footRegion[2], "ALL", 0, 0, 0xf, fill);
    } else {
      font_string_region_clip(footRegion[2], "FILT", 0, 0, 0xf, fill);
    }
  } else {
    font_string_region_clip(footRegion[2], "CLEAR", 0, 0, 0xf, fill);
  }
  */
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
  /*
  if(writeConfirm) {
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
  */
}



// function keys
void handle_key_0(s32 val) {
  // load module
}

void handle_key_1(s32 val) {
  // render status to head region
  
  render_update();
  
  // write module as default 
  files_store_default_dsp(curPage->select);
  
  // render status to head region
  render_update();
}

void handle_key_2(s32 val) {
}

void handle_key_3(s32 val) {
}

// enc 0 : scroll page
void handle_enc_0(s32 val) {
  // nothing
}

// enc 1 : scroll selection
void handle_enc_1(s32 val) {
  // nothing
}

void handle_enc_2(s32 val) {
  // scroll page
  if(val > 0) {
    set_page(ePageIns);
  } else {
    set_page(ePageOuts);
  }
}

void handle_enc_3(s32 val) {
  select_scroll(val);
}

//----------------------
// ---- extern 
// init
void init_page_dsp(void) {
  u8 i, n;
  u16 max = files_get_dsp_count() - 1;
  print_dbg("\r\n alloc DSP page");
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
  // also, reset scroller!
  while(i<5) {
    render_line(i, max);
    render_to_scroll_line(n, i == 0 ? 1 : 0);
    ++n;
    ++i;
  }
}
 
// refresh 
void refresh_dsp(void) {
  print_dbg("\r\n refresh DSP... ");
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "DSP", 0, 0, 0xf, 0x1);
  print_dbg("\r\n finished DSP page refresh");
}
