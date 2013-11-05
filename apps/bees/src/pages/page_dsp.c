/*
  page_dsp.c
 */

// asf
#include "print_funcs.h"

// aleph-avr32
#include "bfin.h"

// bees
#include "files.h"
#include "handler.h"
#include "net.h"
#include "pages.h"
#include "render.h"

//-------------------------
//---- static variables

// scroll region
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };
// scroll manager
static scroll centerScroll;

// handler declarations
static void handle_enc_0(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

/* // array of handlers */
/* const page_handler_t handler_dsp[eNumPageHandlers] = { */
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
static void render_line(s16 idx, u16 max) {
  region_fill(lineRegion, 0x0);
  if((idx > max) || (idx < 0)) { return; }
  clearln();
  appendln((const char*)files_get_dsp_name(idx));
  endln();
  font_string_region_clip(lineRegion, lineBuf, 0, 0, 0xa, 0);
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
  region_fill(footRegion[0], fill);
  font_string_region_clip(footRegion[0], "LOAD", 0, 0, 0xf, fill);
 }

static void show_foot1(void) {
  u8 fill = 0;
  if(keyPressed == 1) {
    fill = 0x5;
  }
  region_fill(footRegion[1], fill);
  font_string_region_clip(footRegion[1], "WRITE", 0, 0, 0xf, fill);
  
}


static void show_foot(void) {
  show_foot0();
  show_foot1();
}


// function keys
void handle_key_0(s32 val) {
  // load module
  if(val == 0) { return; }
  if(check_key(0)) {
    region_fill(headRegion, 0x0);
    font_string_region_clip(headRegion, "loading DSP module...", 0, 0, 0xa, 0);
    headRegion->dirty = 1;
    render_update();
    
    files_load_dsp(curPage->select);
    bfin_wait_ready();
    net_report_params();
    bfin_enable();
    // render status to head region 
    region_fill(headRegion, 0x0);
    font_string_region_clip(headRegion, "finished loading.", 0, 0, 0xa, 0);
    headRegion->dirty = 1;
    render_update();

  }
  show_foot();
}

void handle_key_1(s32 val) {
  if(val == 0) { return; }
  if(check_key(1)) {
    // render status to head region  
    region_fill(headRegion, 0x0);
    font_string_region_clip(headRegion, "writing DSP module -> flash...", 0, 0, 0xa, 0);
    headRegion->dirty = 1;
    render_update();
    // write module as default 
    files_store_default_dsp(curPage->select);
    // render status to head region 
    region_fill(headRegion, 0x0);
    font_string_region_clip(headRegion, "finished writing.", 0, 0, 0xa, 0);
    headRegion->dirty = 1;
    render_update();
  }
  show_foot();
}

void handle_key_2(s32 val) {
  // nothing
}

void handle_key_3(s32 val) {
  // nothing
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
    set_page(ePageOps);
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
 
// select 
void select_dsp(void) {
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "DSP", 0, 0, 0xf, 0x1);
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
