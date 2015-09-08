/*
  page_ops.c
 */

// ASF
#include "print_funcs.h"

// bees
#include "handler.h"
#include "net.h"
#include "op.h"
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

static s16* const pageSelect = &(pages[ePageOps].select);

// current selected new operator type
static op_id_t newOpType = eOpAccum;

//==========================================
//===== static function declarations

// handler declarations
static void handle_enc_3(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_0(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// render a given line
static void render_line(s16 idx);
// scroll the current selection
static void select_scroll(s8 dir);
// render new operator type name
static void render_op_type(void);
// redraw all lines, based on current selection
//static void redraw_lines(void);

//=================================
//==== static definitions

// render a given line
void render_line(s16 idx) {

  region_fill(lineRegion, 0x0);
  if((idx >= 0) && (idx < net_num_ops()) ) {
    clearln();
    appendln_idx_lj((u8)idx);
    appendln_char('.');
    appendln(net_op_name(idx));
    endln();
    font_string_region_clip(lineRegion, lineBuf, 0, 0, 0xa, 0);
    // region_fill_part(lineRegion, LINE_UNDERLINE_OFFSET, LINE_UNDERLINE_LEN, 0x1);
  }
}

// scroll the current selection
void select_scroll(s8 dir) {
  const s32 max = net_num_ops() - 1;
  s16 newSel;
  s16 newIdx;

  // cancel actions
  pages_reset_keypressed();

  if(dir < 0) {
    /// SCROLL DOWN
    // if selection is already zero, do nothing 
    if(*pageSelect == 0) {
      //      print_dbg("\r\n reached min selection in inputs scroll. ");
      return;
    }
    // remove highlight from old center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);
    // decrement selection
    newSel = *pageSelect - 1;
    ///// these bounds checks shouldn't really be needed here...
    //    if(newSel < 0) { newSel = 0; }
    //    if(newSel > max ) { newSel = max; }
    *pageSelect = newSel;    
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
    if(*pageSelect == max) {
      //      print_dbg("\r\n reached max selection in inputs scroll. ");
      return;
    }
    // remove highlight from old center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);
    // increment selection
    newSel = *pageSelect + 1;
    *pageSelect = newSel;    
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
}

// render new operator type name
void render_op_type(void) {
  const char* name = op_registry[userOpTypes[newOpType]].name;
  //  print_dbg("\r\n new op selection: ");
  //  print_dbg(name);
  region_fill(headRegion, 0x0);
  clearln();
  appendln_char('+');
  appendln(name);
  endln();
  font_string_region_clip(headRegion, lineBuf, 0, 0, 0xa, 0);
}


// display the function key labels according to current state
static void show_foot0(void) {
  u8 fill = 0;

  if(keyPressed == 0) {
    fill = 0x5;
  }
  region_fill(footRegion[0], fill);
  font_string_region_clip(footRegion[0], "INS", 0, 0, 0xf, fill);
}

static void show_foot1(void) {
  u8 fill = 0;
  if(keyPressed == 1) {
    fill = 0x5;
  }
  region_fill(footRegion[1], fill);
  font_string_region_clip(footRegion[1], "OUTS", 0, 0, 0xf, fill);
}

static void show_foot2(void) {
  u8 fill = 0;
  if(keyPressed == 2) {
    fill = 0x5;
  }
  region_fill(footRegion[2], fill);
  if(altMode) {
    font_string_region_clip(footRegion[2], "DELETE", 0, 0, 0xf, fill);
  } else {
    font_string_region_clip(footRegion[2], "CREATE", 0, 0, 0xf, fill);
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
  show_foot0();
  show_foot1();
  show_foot2();
  show_foot3();
}

// function keys
void handle_key_0(s32 val) {
  if(val == 0) { return; }
  if(check_key(0)) {
    // select op's inputs on ins page
    pages[ePageIns].select = net_op_in_idx(*pageSelect, 0);
    print_dbg("\r\n got 1st input index for selected op ( ");
    print_dbg_ulong( *pageSelect );
    print_dbg(", result : ");
    print_dbg_ulong( net_op_in_idx(*pageSelect, 0));
    // go to inputs page
    set_page(ePageIns);
    // reset keypress 
    keyPressed = 255;
    redraw_ins();
  }
  show_foot();
}

void handle_key_1(s32 val) {
  if(val == 0) { return; }
  if(check_key(1)) {

    print_dbg("\r\n got 1st output index for selected op ( ");
    print_dbg_ulong( *pageSelect );
    print_dbg(", result : ");
    print_dbg_ulong( net_op_out_idx(*pageSelect, 0));

    // select op's outputs on outs page
    pages[ePageOuts].select = net_op_out_idx(*pageSelect, 0);

    print_dbg("\r\n performed set-selection");

    // go to outputs page
    set_page(ePageOuts);
    // reset keypress
    keyPressed = 255;
    print_dbg("\r\n performed select-page");
    redraw_outs();
    print_dbg("\r\n performed redraw");
  }
  show_foot();
}

void handle_key_2(s32 val) {
  if(val == 0) { return; }
  if(check_key(2)) { 
    if(altMode) {
      // delete last created operator
      net_pop_op();
    } else {
      // create new operator of selected type
      net_add_op(userOpTypes[newOpType]);
      // change selection to last op
      *pageSelect = net_num_ops() - 1;

    }
    redraw_ins();
    redraw_outs();
    redraw_ops();
  }
  show_foot();
}

void handle_key_3(s32 val) {
  altMode = (val > 0);
  show_foot();
}

void handle_enc_3(s32 val) {
  // select new operator type
  if(val > 0) {
    newOpType++;
    if (newOpType >= NUM_USER_OP_TYPES) {
      newOpType = 0;
    }
  } else {
    newOpType--;
    if (newOpType >= NUM_USER_OP_TYPES) {
      newOpType = NUM_USER_OP_TYPES - 1;
    }
  }
  render_op_type();
}

void handle_enc_2(s32 val) {
  // nothing
}

void handle_enc_1(s32 val) {
  // scroll page
  if(val > 0) {
    set_page(ePageIns);
  } else {
    set_page(ePageDsp);
  }
}

void handle_enc_0(s32 val) {
  // scroll selection
  select_scroll(val);
}


//=================================
//==== extern definitions
// init
void init_page_ops(void) {
  u8 i, n;
  print_dbg("\r\n alloc OPS page");
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
    /* print_dbg("\r\n init ops page, line "); */
    /* print_dbg_ulong(i); */
    render_line(i);
    render_to_scroll_line(n, i == 0 ? 1 : 0);
    ++n;
    ++i;
  }
  print_dbg("\r\n done.");
}

// select 
void select_ops(void) { 
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "OPERATORS", 0, 0, 0xf, 0x1);
  show_foot();
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

// redraw all lines, based on current selection
void redraw_ops(void) {
  u8 i=0;
  u8 n = *pageSelect - 3;


  // set scroll region
  // FIXME: should be separate function i guess
  render_set_scroll(&centerScroll);

  while(i<8) {
    render_line( n );
    render_to_scroll_line(i, n == *pageSelect  ? 1 : 0);
    ++i;
    ++n;
  }
}
