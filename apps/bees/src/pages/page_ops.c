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

// const array of user-creatable operator types
#define NUM_USER_OP_TYPES 6
static const op_id_t userOpTypes[NUM_USER_OP_TYPES] = {
  eOpAdd,
  eOpMul,
  eOpGate,
  eOpMonomeGridRaw,
  eOpMidiNote,
  eOpMetro,
  // many more...
  // eOpAccum,
  // eOpSelect,
};

// current selected new operator type
static op_id_t newOpType = 0;

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

// render a given line
static void render_line(s16 idx);
// scroll the current selection
static void select_scroll(s8 dir);
// render new operator type name
static void render_op_type(void);

//=================================
//==== static definitions

// render a given line
void render_line(s16 idx) {
  region_fill(lineRegion, 0x0);
  clearln();
  appendln_idx_lj((u8)idx);
  appendln_char('.');
  appendln(net_op_name(idx));
  endln();
  font_string_region_clip(lineRegion, lineBuf, 0, 0, 0xa, 0);
  region_fill_part(lineRegion, LINE_UNDERLINE_OFFSET, LINE_UNDERLINE_LEN, 0x1);
}

// scroll the current selection
void select_scroll(s8 dir) {
  const s32 max = net_num_ops() - 1;
  s16 newSel;
  s16 newIdx;
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
    curPage->select = newSel;    
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
  font_string_region_clip(footRegion[2], "CREATE", 0, 0, 0xf, fill);
  
}

static void show_foot3(void) {
  u8 fill = 0;
  //  u8 fore = 0xf;
  //..???
  region_fill(footRegion[3], fill);
  //font_string_region_clip(footRegion[3], "ALT", 0, 0, fore, fill);
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
    //...
  }
}

void handle_key_1(s32 val) {
  if(val == 0) { return; }
  if(check_key(1)) {
    // select op's outputs on outs page
    //...
  }
}

void handle_key_2(s32 val) {
  if(val == 0) { return; }
  if(check_key(2)) { 
    // create new operator of selected type
    net_add_op(userOpTypes[newOpType]);
    // redraw...
  }
  show_foot();
}

void handle_key_3(s32 val) {
  // delete? subpage?
}

void handle_enc_0(s32 val) {
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

void handle_enc_1(s32 val) {
  // nothing
}

void handle_enc_2(s32 val) {
  // scroll page
  if(val > 0) {
    set_page(ePageIns);
  } else {
    set_page(ePageDsp);
  }
}

void handle_enc_3(s32 val) {
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
    render_line(i);
    render_to_scroll_line(n, i == 0 ? 1 : 0);
    ++n;
    ++i;
  }
}

// select 
void select_ops(void) { 
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "OPERATORS", 0, 0, 0xf, 0x1);
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
