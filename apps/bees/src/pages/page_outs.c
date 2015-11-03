/*
  page_outs.c
 */

// asf
#include "print_funcs.h"

// bees
#include "handler.h"
#include "net.h"
#include "pages.h"
#include "preset.h"
#include "render.h"

//====================================
//==== static variables

//--- these are common to all pages
// a region for the center scroll
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };
// a scroll class that manages write/read offsets into the scroll region
static scroll centerScroll;

//--- page-specific state variables
// we are in preset=selection momentary mode
static u8 inPresetSelect = 0;

// in clear operation
static u8 clearConfirm = 0;
// privisional target selections16  
static s16 tmpTarget = -1;
// in-target-select mode
static u8 targetSelect = 0;

// kludge:
// constant pointer to this page's selection
static s16* const pageSelect = &(pages[ePageOuts].select);

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

// show footers
static void show_foot0(void);
static void show_foot1(void);
static void show_foot2(void);
static void show_foot3(void);
static void show_foot(void);

// redraw based on provisional preset seleciton
static void redraw_outs_preset (void);

// fill tmp region with new content
// given input index and foreground color
static void render_line(s16 idx, u8 fg) {
  //  const s16 opIdx = net_in_op_idx(idx);
  s16 target;
  s16 targetOpIdx = -1;
  s16 srcOpIdx; 
  region_fill(lineRegion, 0x0);

  //  print_dbg("\r\n page_outs: render_line");
  if(idx >= net_num_outs() ) { return; }
  if(targetSelect) { 
    //      print_dbg(" , in targetSelect");
    target = tmpTarget;
  } else {
    target = net_get_target(idx);
  }
  srcOpIdx = net_out_op_idx(idx);
  targetOpIdx = net_in_op_idx(target);

  /* print_dbg(" , target: "); */
  /* print_dbg_ulong(target); */

  if(target >= 0) {
    //// output has target
    // the network doesn't actually execute connections from an op to itself.
    // reflect this in UI by dimming this line
    if(targetOpIdx == srcOpIdx) { fg = 0x5; }
    // render output
    clearln();
    appendln_idx_lj(srcOpIdx);
    appendln_char('.');
    appendln( net_op_name(srcOpIdx));
    appendln_char('/');
    appendln( net_out_name(idx) );
    endln();
    font_string_region_clip(lineRegion, lineBuf, 2, 0, fg, 0);
    // render target
    targetOpIdx = net_in_op_idx(target);
    clearln();
    appendln("-> ");
    if(targetOpIdx >= 0) {
      //      print_dbg(" , target is op in");
      // target is operator input
      appendln_idx_lj(net_in_op_idx(target));
      appendln_char('.');
      appendln( net_op_name(net_in_op_idx(target)) );
      appendln_char('/');
      appendln( net_in_name(target) );
    } else {
      //      print_dbg(" , target is param in");
      // target is parameter input
      appendln_idx_lj( (int)net_param_idx(target)); 
      appendln_char('.');
      appendln( net_in_name(target)); 
    }
    endln();
    font_string_region_clip(lineRegion, lineBuf, 60, 0, fg, 0);
    clearln();
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
    font_string_region_clip(lineRegion, lineBuf, 2, 0, fg, 0);
  }
  // draw something to indicate preset inclusion
  if(net_get_out_preset(idx)) {
    font_string_region_clip(lineRegion, ".", 125, 0, fg, 0);
  }
  // underline
  //  region_fill_part(lineRegion, LINE_UNDERLINE_FSET, LINE_UNDERLINE_LEN, 0x1);
}

// edit the current seleciton
static void select_edit(s32 inc) {
  s16 tmptmp;
  //  print_dbg("\r\n page_outs: select_edit");
  // enter target-select mode
  if(targetSelect == 0) {
    //    print_dbg(" , set targetSelect mode");
    targetSelect = 1;
    /// only change tmp target selection if connected
    /// thus, unconnected outputs should default in editor to last connection made.
    tmptmp = net_get_target(*pageSelect);
    if(tmptmp > -1) {
      tmpTarget = tmptmp;
    }
  }
  /* print_dbg("\r\n tmpTarget: "); */
  /* print_dbg_ulong(tmpTarget); */
  //  if(inc > 0) {
    /* print_dbg(" , inc tmpTarget"); */
    /* print_dbg(" , value: "); */
    /* print_dbg_ulong(tmpTarget); */
    // increment tmpTarget
    tmpTarget += inc;
    if(tmpTarget >= net_num_ins() ) {
      tmpTarget -= (net_num_ins() + 1);
    }
    if(tmpTarget < -1) {
      tmpTarget += (net_num_ins() + 1);
    }
    /* ++tmpTarget; */
    /* if(tmpTarget == net_num_ins()) { */
    /*   //      print_dbg(" , tmpTarget at max"); */
    /*   // scroll past all inputs : disconnect and wrap */
    /*   tmpTarget = -1; */
    /* }  */
  /* } else { */
  /*   --tmpTarget; */
  /*   if (tmpTarget == -2) { */
  /*     //      print_dbg(" , tmpTarget at min"); */
  /*     //  scrolled down from disconnect: connect and wrap */
  /*     tmpTarget = net_num_ins() - 1; */
  /*   } */
    /* } */
  
  // render to tmp buffer
  render_line(*pageSelect, 0xf);
  // copy to scroll with highlight
  render_to_scroll_line(SCROLL_CENTER_LINE, 1);
  show_foot();
  //  }
}

#if 1 


// scroll the current selection
static void select_scroll(s32 dir) {
  const s32 max = net_num_outs() - 1;
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

  oldSel = *pageSelect;
  *pageSelect = newSel;

  // remove highlight from old center
  render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);

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
	/* print_dbg(" , rendering new line for idx: "); */
	/* print_dbg_ulong(newIdx); */
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
	/* print_dbg(" , rendering new line for idx: "); */
	/* print_dbg_ulong(newIdx); */
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

#else 
/* // scroll the current selection */
/* static void select_scroll(s32 dir) { */
/*   const s32 max = net_num_outs() - 1; */
/*   // index for new content */
/*   s16 newIdx; */
/*   s16 newSel; */

/*   targetSelect = 0; */
  
/*   if(dir < 0) { */
/*     /// SCROLL DOWN */
/*     // if selection is already zero, do nothing  */
/*     if(*pageSelect == 0) { */
/*       //      print_dbg("\r\n reached min selection in inputs scroll. "); */
/*       return; */
/*     } */
/*     // remove highlight from old center */
/*     render_scroll_apply_hl(SCROLL_CENTER_LINE, 0); */
/*     // decrement selection */
/*     newSel = *pageSelect - 1; */
/*     ///// these bounds checks shouldn't really be needed here... */
/*     //    if(newSel < 0) { newSel = 0; } */
/*     //    if(newSel > max ) { newSel = max; } */
/*     *pageSelect = newSel;     */
/*     newIdx = newSel - SCROLL_LINES_BELOW; */
/*     if(newIdx < 0) {  */
/*       // empty row */
/*       region_fill(lineRegion, 0); */
/*     } else { */
/*       render_line(newIdx, 0xa); */
/*     } */
/*     // render tmp region to bottom of scroll */
/*     // (this also updates scroll byte offset)  */
/*     render_to_scroll_top(); */
/*     // add highlight to new center */
/*     render_scroll_apply_hl(SCROLL_CENTER_LINE, 1); */

/*   } else { */
/*     // SCROLL UP */
/*     // if selection is already max, do nothing  */
/*     if(*pageSelect == max) { */
/*       //      print_dbg("\r\n reached max selection in inputs scroll. "); */
/*       return; */
/*     } */
/*     // remove highlight from old center */
/*     render_scroll_apply_hl(SCROLL_CENTER_LINE, 0); */
/*     // increment selection */
/*     newSel = *pageSelect + 1; */
/*     ///// these bounds checks shouldn't really be needed here... */
/*     //    if(newSel < 0) { newSel = 0; } */
/*     //    if(newSel > max ) { newSel = max; } */
/*     ///// */
/*     *pageSelect = newSel;     */
/*     // add new content at bottom of screen */
/*     newIdx = newSel + SCROLL_LINES_ABOVE; */
/*     if(newIdx > max) {  */
/*       // empty row */
/*       region_fill(lineRegion, 0); */
/*     } else { */
/*       render_line(newIdx, 0xa); */
/*     } */
/*     // render tmp region to bottom of scroll */
/*     // (this also updates scroll byte offset)  */
/*     render_to_scroll_bottom(); */
/*     // add highlight to new center */
/*     render_scroll_apply_hl(SCROLL_CENTER_LINE, 1); */
/*   } */
/* } */
#endif

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
    if(net_get_out_preset((u32)(*pageSelect))) {
      //    if(inPreset) {
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
  if(targetSelect) {
    font_string_region_clip(footRegion[2], "CONN", 0, 0, 0xf, fill);
  } else {
    font_string_region_clip(footRegion[2], "DISC", 0, 0, 0xf, fill);
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
    render_line(i, 0xa);
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

// function key handlers
void handle_key_0(s32 val) {
  if(val == 0) { return; }
  if(altMode) {
    ///// follow
    // select target on ins page
    tmpTarget = net_get_target(*pageSelect);
    if(tmpTarget >= 0) {
      pages[ePageIns].select = tmpTarget;
      set_page(ePageIns);
      redraw_ins();
    }
  } else {
    // store
    // show selected preset name
    draw_preset_name();
    if(check_key(0)) {
      // store in preset
      net_set_out_preset(*pageSelect, 1);
      preset_store_out(preset_get_select(), *pageSelect);
      // redraw selected line
      render_line(*pageSelect, 0xa);
      render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);
      // TODO: store directly in scene?
    }
  }
  show_foot();
}

void handle_key_1(s32 val) {
  s16 newOut;
  if(val == 0) { return; }
  if(check_key(1)) {
    if(altMode) {
      print_dbg("\r\n splitting output: ");
      print_dbg_ulong(*pageSelect);
      newOut = net_split_out(*pageSelect);
      *pageSelect = newOut;
      redraw_outs();
    } else {
      // include / exclude in selected preset
	// show preset name in head region
      draw_preset_name();
      // include / exclude in preset
      net_toggle_out_preset(*pageSelect);
      // re-draw selected line to update inclusion glyph
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
    if(targetSelect) {
      // we are selecting a target, so perform the connection
      net_connect(*pageSelect, tmpTarget);
      targetSelect = 0;
    } else {
      // not selecting, clear current connection
      net_disconnect(*pageSelect);
      // re-draw selected line 
      render_line(*pageSelect, 0xf);
      // copy to scroll with hi,ghlight
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
      redraw_outs();
    }
  }
  show_foot();
}

// encoder handlers
void handle_enc_3(s32 val) {   
  // edit selection / target
  if(*pageSelect != -1) {
    if(val > 0) {
      if(altMode) {
	select_edit(7);
      } else {
	select_edit(1);
      }
    } else {
    if(altMode) {
      select_edit(-7);
    } else {
      select_edit(-1);
    }
  }
  } 
}

void handle_enc_2(s32 val) {
  if(targetSelect) {
    targetSelect = 0;
    redraw_outs();
  }
  if(altMode) {
    inPresetSelect = 1;
    if(val > 0) {
      preset_inc_select(1);
    } else {
      preset_inc_select(-1);
    }
    // refresh line data
    redraw_outs_preset();
  } else {

  /* if(targetSelect) { */
  /*   targetSelect = 0; */
  /*   redraw_outs(); */
  /* } */
   ;;  // nothing to do ... kind of weird/dumb
  }
}

void handle_enc_1(s32 val) {  
  if(targetSelect) {
    targetSelect = 0;
    redraw_outs();
  }
  // scroll page
  if(val > 0) {
    set_page(ePagePresets);
  } else {
    set_page(ePageIns);
  }
}

/* void handle_enc_0(s32 val) { */

/*   //  print_dbg("\r\n outs page: handling encoder 3"); */
/*   if(targetSelect) { */
/*     targetSelect = 0; */
/*     redraw_outs(); */
/*   } */

/*   if(altMode) { */
/*     inPresetSelect = 1; */
/*     if(val > 0) { */
/*       preset_inc_select(1); */
/*     } else { */
/*       preset_inc_select(-1); */
/*     } */
/*     // refresh line data */
/*     redraw_outs_preset(); */
/*   } else { */
/*     // scroll selection */
/*     select_scroll(val); */
/*   } */
/* } */



void handle_enc_0(s32 val) {
  if(targetSelect) {
    targetSelect = 0;
    redraw_outs();
  }
  if(altMode) {
    // alt: page selection			
    select_scroll(val > 0 ? 7 : -7);
    //    redraw_ins();
  } else {
    // scroll selection
    select_scroll(val > 0 ? 1 : -1);
  }
}


// redraw all lines, based on current selection
void redraw_outs(void) {
  u8 i=0;
  u8 n = *pageSelect - 3;

  // set scroll region
  // FIXME: should be separate function i guess
  render_set_scroll(&centerScroll);

  print_dbg("\r\n redraw_outs() ");

  while(i<8) {
    /* print_dbg("\r\n redraw_outs, line: "); */
    /* print_dbg_ulong(i); */
    /* print_dbg("index: "); */
    /* print_dbg_ulong(n); */

    render_line( n, 0xa );
    render_to_scroll_line(i, n == *pageSelect ? 1 : 0);
    ++i;
    ++n;
  }
}

// redraw based on provisional preset seleciton
void redraw_outs_preset (void) {
  //  s32 max = net_num_outs() - 1;

  /*
  u8 i=0;
  u8 idx = *pageSelect - 3;
  u8 fg;
  u8 enabled;
  s16 target;
  s16 targetOpIdx = -1;
  s16 srcOpIdx; 
  */

  //  s32 preSel = preset_get_select();

  print_dbg("\r\n redraw_outs_preset()");

  //ppfffaaaggh
  /*
  while(i<8) {
    region_fill(lineRegion, 0x0);
    if(idx >= net_num_outs() ) { return; }

    //    enabled = preset_out_enabled(preSel, idx);
    //??
    enabled = preset_get_selected()->outs[idx].enabled;

    if(enabled) {
      // if it's enabled, show the preset's target (including if no target/disconnection)
      target = preset_get_selected()->outs[idx].target;
      srcOpIdx = net_out_op_idx(idx);
      targetOpIdx = net_in_op_idx(target);
      if(target >= 0) {
	//// output has target
	// the network doesn't actually execute connections from an op to itself.
	// reflect this in UI by dimming this line
	if(targetOpIdx == srcOpIdx) { fg = 0x5; }
	// render output
	clearln();
	appendln_idx_lj(srcOpIdx);
	appendln_char('.');
	appendln( net_op_name(srcOpIdx));
	appendln_char('/');
	appendln( net_out_name(idx) );
	endln();
	font_string_region_clip(lineRegion, lineBuf, 2, 0, fg, 0);
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
	font_string_region_clip(lineRegion, lineBuf, 60, 0, fg, 0);
	clearln();
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
	font_string_region_clip(lineRegion, lineBuf, 2, 0, fg, 0);
      }
      // draw something to indicate preset inclusion
      if(net_get_out_preset(idx)) {
	font_string_region_clip(lineRegion, ".", 126, 0, fg, 0);
      }
    
    } else {
      // not enabled, draw as normal with dim coloring
      render_line(idx, 0x5);
    }

    render_to_scroll_line(i, 0);
    ++i;
    ++idx;
  }
  */
  draw_preset_name();
}
