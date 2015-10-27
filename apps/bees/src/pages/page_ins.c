/*
  page_ins.c
 */

// asf
#include "print_funcs.h"

// common
#include "fix.h"

// avr32
#include "region.h"

// bees
#include "handler.h"
#include "net.h"
#include "pages.h"
#include "preset.h"
#include "render.h"

//-------------------------
//---- static variables

// scroll region
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };
// scroll manager
static scroll centerScroll;


// we are in preset=selection momentary mode
static u8 inPresetSelect = 0;

// in-clear-confirm stateS
static u8 clearConfirm = 0;

// selected input is zeroed
static u8 zeroed = 0;

// kludge:
// constant pointer to this page's selection
static s16* const pageSelect = &(pages[ePageIns].select);

//-------------------------
//---- static declarations

// handlers
static void handle_enc_3(s32 val);
static void handle_enc_2(s32 val);
static void handle_enc_1(s32 val);
static void handle_enc_0(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
static void handle_key_2(s32 val);
static void handle_key_3(s32 val);

// redraw based on provisional preset seleciton
static void redraw_ins_preset(void );

// fill tmp region with new content
// given input index and foreground color
static void render_line(s16 idx, u8 fg) {
  const s16 opIdx = net_in_op_idx(idx);  
  region_fill(lineRegion, 0x0);
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

    font_string_region_clip(lineRegion, lineBuf, 4, 0, fg, 0);
    clearln();
 
    op_print(lineBuf, net_get_in_value(idx));

    font_string_region_clip(lineRegion, lineBuf, LINE_VAL_POS_SHORT, 0, fg, 0);
  } else {
    // parameter input    
    clearln();
    appendln_idx_lj( (int)net_param_idx(idx)); 
    appendln_char('.');
    appendln( net_in_name(idx)); 
    endln();
    font_string_region_clip(lineRegion, lineBuf, 4, 0, 0xa, 0);
    clearln();

    //    op_print(lineBuf, net_get_in_value(idx));
    /// FIXME: this is pretty dumb, 
    // params and inputs should just be on separate pages i guess
    net_get_param_value_string(lineBuf, idx);

    font_string_region_clip(lineRegion, lineBuf, LINE_VAL_POS_LONG, 0, fg, 0);
  }
  // draw something to indicate play mode visibility
  if(net_get_in_play(idx)) {
    font_string_region_clip(lineRegion, ".", 0, 0, fg, 0);
  }
  // draw something to indicate preset inclusion
  if(net_get_in_preset(idx)) {
    font_string_region_clip(lineRegion, ".", 126, 0, fg, 0);
  }

}

// edit the current seleciton
static void select_edit(s32 inc) {

  print_dbg("\r\n editing ins page selection, idx: ");
  print_dbg_ulong(*pageSelect);

  if(*pageSelect != -1) {
    // increment input value
    net_inc_in_value(*pageSelect, inc);
    // render to tmp buffer
    render_line(*pageSelect, 0xf);
    // copy to scroll with highlight
    render_to_scroll_line(SCROLL_CENTER_LINE, 1);
  }
}

// scroll the current selection
static void select_scroll(s32 dir) {
  const s32 max = net_num_ins() - 1;
  // index for new content
  s16 newIdx;
  s16 newSel;
  s16 oldSel;
  int i;

  // cancel actions
  pages_reset_keypressed();


  // wrap with blank line
  newSel = *pageSelect + dir;
  if (newSel < -1) { 
    newSel += (max + 2);
  } 
  if(newSel > max) {
    newSel -= (max + 2);
  }

  //  print_dbg("\r\n scrolled selection on inputs page, old sel: ");
  //  print_dbg_ulong(*pageSelect);
  //  print_dbg(" ,  dir: ");
  //  print_dbg_hex(dir);
  //  print_dbg(" , new idx: ");
  //  print_dbg_ulong(newSel);

  oldSel = *pageSelect;
  *pageSelect = newSel; 
  // remove highlight from old center
  render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);

  // update 'zeroed' flag
  zeroed = (net_get_in_value(*pageSelect) == 0);

  if(dir > 0) { 
    // add content at bottom
    for(i=0; i<dir; ++i) {
      newIdx = oldSel + SCROLL_LINES_BELOW + i + 2;
      
      if(newIdx == (max + 1)) {
	region_fill(lineRegion, 0);
      } else {
	if(newIdx > max) {
	  newIdx = newIdx - (max+2);
	}
	//	print_dbg(" , rendering new line for idx: ");
	//	print_dbg_ulong(newIdx);
	render_line(newIdx, 0xa);
      }
      // render tmp region to bottom of scroll
      // (this also updates scroll byte offset) 
      render_to_scroll_bottom();
      // add highlight to new center
    }
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);
  } else {
    // add content at top
    for(i=0; i>dir; --i) {
      newIdx = oldSel - SCROLL_LINES_ABOVE + i;
      if(newIdx == -1) {
	region_fill(lineRegion, 0);
      } else {
	if(newIdx < -1) {
	newIdx = newIdx + max + 2;
	}
	//	print_dbg(" , rendering new line for idx: ");
	//	print_dbg_ulong(newIdx);
	render_line(newIdx, 0xa);
      }
      // render tmp region to top of scroll
      // (this also updates scroll byte offset) 
      render_to_scroll_top();
    }
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
  if(altMode) {
    /// TODO
    //    font_string_region_clip(footRegion[0], "GATHER", 0, 0, 0xf, fill);
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
    if(net_get_in_play(*pageSelect)) {
      font_string_region_clip(footRegion[1], "HIDE", 0, 0, 0xf, fill);
    } else {
      font_string_region_clip(footRegion[1], "SHOW", 0, 0, 0xf, fill);
    }
  } else {
    if(net_get_in_preset(*pageSelect)) {
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
#if 0
    if(playFilter) {
      font_string_region_clip(footRegion[2], "ALL", 0, 0, 0xf, fill);
    } else {
      font_string_region_clip(footRegion[2], "FILT", 0, 0, 0xf, fill);
    }
#endif
  } else {
    if(zeroed) {
      font_string_region_clip(footRegion[2], "MAX", 0, 0, 0xf, fill);
    } else {
      font_string_region_clip(footRegion[2], "ZERO", 0, 0, 0xf, fill);
    }
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
    // we don't ever really update the foot regions independently.
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

//----------------------
// ---- extern 
// init
void init_page_ins(void) {
  u8 i, n;
  print_dbg("\r\n alloc INS page");
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

// selected
// called when the page is selected
// assign rendering buffers and UI handlers 
void select_ins(void) { 
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "INPUTS", 0, 0, 0xf, 0x1);
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

  // redraw for new values
  redraw_ins();

}

//========================================================
//======= static definitions


//------------------------
//-- handlers
// function keys
void handle_key_0(s32 val) {
  if(val == 0) { return; }

  if(altMode) {
    // gather
    /// TODO
  } else {
    // show selected preset name
    draw_preset_name();
    if(check_key(0)) {
      // store in preset
      net_set_in_preset(*pageSelect, 1);
      preset_store_in(preset_get_select(), *pageSelect);
      // redraw selected line
      render_line(*pageSelect, 0xf);
      render_to_scroll_line(SCROLL_CENTER_LINE, 1);
    }
  }
  show_foot();
}

void handle_key_1(s32 val) {
  if(val == 0) { return; }
    if(altMode) {
      if(check_key(1)) {
	// show / hide on play screen
	net_toggle_in_play(*pageSelect);
	// render to tmp buffer
	render_line(*pageSelect, 0xf);
	// copy to scroll with highlight
	render_to_scroll_line(SCROLL_CENTER_LINE, 1);
      }
    } else {
      if(check_key(1)) {
	// show preset name in head region
	draw_preset_name();
	// include / exclude in preset
	net_toggle_in_preset(*pageSelect);
	// render to tmp buffer
	render_line(*pageSelect, 0xf);
	// copy to scroll with highlight
	render_to_scroll_line(SCROLL_CENTER_LINE, 1);
      }
    }
    show_foot();
}

void handle_key_2(s32 val) {
  if(val == 0) { return; }
  if(check_key(2)) {
    if(altMode) {
      // filter / all
    } else {
      if(zeroed) { 
	/// set to max
	net_set_in_value(*pageSelect, OP_MAX_VAL);
	zeroed = 0;
      } else {
	/// set to 0
	net_set_in_value(*pageSelect, 0);
	zeroed = 1;
      }
      // render to tmp buffer
      render_line(*pageSelect, 0xf);
      // copy to scroll with highlight
      render_to_scroll_line(SCROLL_CENTER_LINE, 1);
    }
  } 
  show_foot();
}

void handle_key_3(s32 val) {
  // alt mode
  altMode = (val > 0); 
  if(altMode) { 
    if(inPresetSelect) {
      // load selected preset
      print_dbg("\r\n recalling preset from ins page, idx:");
      print_dbg_ulong(preset_get_select());
      preset_recall(preset_get_select());
      inPresetSelect = 0;
    }
  }
  // always redraw to refresh values
  redraw_ins();
  show_foot();
}

void handle_enc_3(s32 val) {
  // change parameter value, accelerated
  if(*pageSelect != -1) {
    //    select_edit( (scale_knob_value(val) ) );
    select_edit( (scale_knob_value_fast(val) ) );
  }
}

void handle_enc_2(s32 val) {
  if(altMode) {
    // alt:scroll preset
    inPresetSelect = 1;
    if(val > 0) {
      preset_inc_select(1);
    } else {
      preset_inc_select(-1);
    }
    // refresh line data
    redraw_ins_preset();
  } else {
    // change parameter value, unaccelerated
    if(*pageSelect != -1) {      
      select_edit(scale_knob_value_small(val));
    }
  }
}

void handle_enc_1(s32 val) {
  // scroll page
  if(val > 0) {
    set_page(ePageOuts);
  } else {
    set_page(ePageOps);
  }
}

void handle_enc_0(s32 val) {
  if(altMode) {
    // alt: page selection			
    select_scroll(val > 0 ? 7 : -7);
    //    redraw_ins();
  } else {
    // scroll selection
    select_scroll(val > 0 ? 1 : -1);
  }
}

// redraw all lines, force selection
void redraw_ins(void) {
  u8 i=0;
  s32 n = *pageSelect - 3;
  // num_ins() includes param count!
  const s32 max = net_num_ins() - 1;

  // set scroll region
  // FIXME: should be separate function i guess
  render_set_scroll(&centerScroll);

  print_dbg("\r\n redraw_ins() ");

  while(i<8) {

    if(n == -1) {
      // draw a blank line
      region_fill(lineRegion, 0);
    } else if(n == (max+1)) {
      // draw a blank line
      region_fill(lineRegion, 0);
    } else {
      if(n < -1) {
	n += (max + 2);
      }
      if( n > max ) {
	n -= (max + 2);
      }

      render_line( n, 0xa );
    }
    render_to_scroll_line(i, n == *pageSelect ? 1 : 0);
    ++i;
    ++n;
  }
}

// redraw based on provisional preset seleciton
void redraw_ins_preset ( void ) {
  s32 max = net_num_ins() - 1;
  u8 i=0;
  u8 n = *pageSelect - 3;
  u8 enabled;
  io_t opVal;
  s32 paramVal;
  s16 opIdx;


  print_dbg("\r\n redraw_ins_preset() ");


  while(i<8) {
    region_fill(lineRegion, 0x0);

    opIdx = net_in_op_idx(n);  

    if(n <= max) {
      enabled = net_get_in_preset(n);
      if(opIdx < 0 ) {
	// parameter...
	clearln();
	appendln_idx_lj( (int)net_param_idx(n)); 
	appendln_char('.');
	appendln( net_in_name(n)) ; 
	endln();
	font_string_region_clip(lineRegion, lineBuf, 4, 0, 0xf, 0);
	clearln();

	if(enabled) {
	  paramVal = preset_get_selected()->ins[n].value;
	  net_get_param_value_string_conversion(lineBuf, net_param_idx(n), paramVal);
	} else {
	  net_get_param_value_string(lineBuf, n);
	}
	font_string_region_clip(lineRegion, lineBuf, LINE_VAL_POS_LONG, 0, 0xf, 0);
      } else {
	// op input
	clearln();
	appendln_idx_lj(opIdx);
	appendln_char('.');
	appendln( net_op_name(opIdx) );
	appendln_char('/');
	appendln( net_in_name(n) );
	endln();

	font_string_region_clip(lineRegion, lineBuf, 4, 0, 0xf, 0);

	if(enabled) {
	  opVal = preset_get_selected()->ins[n].value;
	} else {
	  opVal = net_get_in_value(n);
	}
	op_print(lineBuf, opVal);

	font_string_region_clip(lineRegion, lineBuf, LINE_VAL_POS_SHORT, 0, 0xf, 0);
      }
      // draw something to indicate preset inclusion
      if(enabled) {
	font_string_region_clip(lineRegion, ".", 126, 0, 0xf, 0);
      }
    }
    render_to_scroll_line(i, 0);
    ++i;
    ++n;
  }
  //  print_dbg("\r\n\r\n");
  draw_preset_name();
}
