/*
  page_presets.c
 */

#include "print_funcs.h"

#include "files.h"
#include "handler.h"
#include "net.h"
#include "pages.h"
#include "render.h"
#include "preset.h"


//------------------------------
// static vars

//--- these are common to all pages
// a region for the center scroll
static region scrollRegion = { .w = 128, .h = 64, .x = 0, .y = 0 };
// a scroll class that manages write/read offsets into the scroll region
static scroll centerScroll;

//--- page-specific state variables
// clear flag
static u8 inClear = 0;
// copy flag
static u8 inCopy = 0;
// editing cursor position
static s8 cursor = 4;
// max index
static const s32 maxPresetIdx = NET_PRESETS_MAX - 1;

// constant pointer to this page's selection
static s16* const pageSelect = &(pages[ePagePresets].select);

//-------------------------
//---- static funcs

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


// render line
static void render_line(s16 idx, u8 fg);
// scroll the current selection
static void select_scroll(s32 dir);

static void redraw_lines(void);
static void render_selection(void);

// fill tmp region with new content
// given input index and foreground color
static void render_line(s16 idx, u8 fg) {
  region_fill(lineRegion, 0x0);
  if( (idx >= 0) && (idx <= maxPresetIdx) ) {
    clearln();
    appendln((const char*)preset_name(idx));
    endln();
    font_string_region_clip(lineRegion, lineBuf, 2, 0, fg, 0);
  }
}

// scroll the current selection
static void select_scroll(s32 dir) {
  
  // index for new content
  s16 newIdx;
  s16 newSel;

  // cancel actions
  pages_reset_keypressed();
  cursor = 4;

  if(dir < 0) {
    /// SCROLL DOWN
    if(*pageSelect == 0) {
      return;
    }
    /* // remove highlight from old center */
    /* render_scroll_apply_hl(SCROLL_CENTER_LINE, 0); */

    // redraw and re-render old center selection
     /// hm, why is this wrong.. 
    render_line(*pageSelect, 0xa);
    render_to_scroll_line(SCROLL_CENTER_LINE, 0); 

    // decrement selection
    newSel = *pageSelect - 1;
    *pageSelect = newSel;    
    // add new content at top
    newIdx = newSel - SCROLL_LINES_BELOW;
    if(newIdx < 0) { 
      // empty row
      region_fill(lineRegion, 0);
    } else {
      render_line(newIdx, 0xa);
    }
    // render tmp region to bottom of scroll
    // (this also updates scroll byte offset) 
    render_to_scroll_top();
    // add highlight to new center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);

  } else {
    // SCROLL UP
    // if selection is already max, do nothing 
    if(*pageSelect == (maxPresetIdx) ) {
      return;
    }
    // remove highlight from old center
    // render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);
    // re-render last center without cursor
    render_line(*pageSelect, 0xa);
    render_to_scroll_line(SCROLL_CENTER_LINE, 0); 
    
    // increment selection
    newSel = *pageSelect + 1;
    *pageSelect = newSel;    
    // add new content at bottom of screen
    newIdx = newSel + SCROLL_LINES_ABOVE;
    if(newIdx > maxPresetIdx) { 
      // empty row
      region_fill(lineRegion, 0);
    } else {
      render_line(newIdx, 0xa);
    }
    // render tmp region to bottom of scroll
    // (this also updates scroll byte offset) 
    render_to_scroll_bottom();
    // add highlight to new center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);
  }
}

static void redraw_lines(void) {
  u8 i=0;
  u8 n = 3;
  while(i<5) {
    render_line(i, 0xa);
    render_to_scroll_line(n, i == 0 ? 1 : 0);
    ++n;
    ++i;
  }
}

//---- function keys

// store
void handle_key_0(s32 val) {
  if(val == 1) { return; }
  if(check_key(0)) {
    preset_store(*pageSelect);
  }
  show_foot();
}

// recall
void handle_key_1(s32 val) {
  if(val == 1) { return; }
  if(check_key(1)) {
    preset_recall(*pageSelect);
  }
  show_foot();
}

// copy / clear / confirm
void handle_key_2(s32 val) {
  if(val == 1) { return; }
  if(check_key(2)) {
    preset_clear(*pageSelect);
  }
  show_foot();
}

// alt
void handle_key_3(s32 val) {
  if ( pages_set_alt(val)) { 
    preset_name(*pageSelect)[cursor] = '\0';
    redraw_presets();
    render_selection();
  }
  show_foot();
}

// scroll character value at cursor positoin in scene name
void handle_enc_3(s32 val) {
  
  if(val > 0) {
    pages_edit_char_inc(preset_name(*pageSelect), cursor);
  } else {
    pages_edit_char_dec(preset_name(*pageSelect), cursor);
  }
  print_dbg("\r\b edited preset name: ");
  print_dbg(preset_name(*pageSelect));
  render_selection();  
}

// scroll cursor position in current scene name
void handle_enc_2(s32 val) {
  pages_edit_cursor(val, preset_name(*pageSelect), &cursor, PRESET_NAME_LEN);
  /* if(val > 0) { */
  /*   ++cursor; */
  /*   if (cursor >= PRESET_NAME_LEN) { */
  /*     cursor = 0; */
  /*   }  */
  /* } else { */
  /*   --cursor; */
  /*   if (cursor < 0) { */
  /*     cursor = PRESET_NAME_LEN - 1; */
  /*   }  */
  /* } */

  /* if(preset_name(*pageSelect)[cursor] == '\0') {  */
  /*   preset_name(*pageSelect)[cursor] = '_'; */
  /* } */
  render_edit_string(lineRegion, preset_name(*pageSelect), PRESET_NAME_LEN, cursor);
  render_to_scroll_line(SCROLL_CENTER_LINE, 0);
  
  
}

// enc 0 : scroll page
void handle_enc_1(s32 val) {
   if(val > 0) {
    set_page(ePageScenes);
  } else {
    set_page(ePageOuts);
  }
}

// enc 1 : scroll selection
void handle_enc_0(s32 val) {
  select_scroll(val);
}

// display the function key labels according to current state
static void show_foot0(void) {
  u8 fill = 0;
  if(keyPressed == 0) {
    fill = 0x5;
  }
  region_fill(footRegion[0], fill);
  font_string_region_clip(footRegion[0], "STORE", 0, 0, 0xf, fill);  
}

static void show_foot1(void) {
  u8 fill = 0;
  if(keyPressed == 1) {
    fill = 0x5;
  }
  region_fill(footRegion[1], fill);
  font_string_region_clip(footRegion[1], "RECALL", 0, 0, 0xf, fill);  

}

static void show_foot2(void) {
  u8 fill = 0;
  if(keyPressed == 2) {
    fill = 0x5;
  }
  region_fill(footRegion[2], fill);

  if(altMode) {
    font_string_region_clip(footRegion[2], "COPY", 0, 0, 0xf, fill);
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
  if(inClear || inCopy) {
    // currently not used
    font_string_region_clip(footRegion[0], "-    ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[1], "-    ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[2], "OK!  ", 0, 0, 0xf, 0);
    font_string_region_clip(footRegion[3], "-    ", 0, 0, 0xf, 0x5);
  } else { 
    show_foot0();
    show_foot1();
    show_foot2();
    show_foot3();
  } 
}

//----------------------
// ---- extern 

// init
void init_page_presets(void) {
  u8 i, n;
  print_dbg("\r\n alloc PRESETS page");
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
  redraw_lines();
}

// select 
void select_presets(void) {
  // assign global scroll region pointer
  // also marks dirty
  render_set_scroll(&centerScroll);
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "PRESETS", 0, 0, 0xf, 0x1);
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
void redraw_presets(void) {
  u8 i=0;
  u8 n = *pageSelect - 3;

  // set scroll region
  // FIXME: should be separate function i guess
  render_set_scroll(&centerScroll);

  while(i<8) {
    render_line( n, 0xa );
    render_to_scroll_line(i, n == *pageSelect ? 1 : 0);
    ++i;
    ++n;
  }
}

void render_selection(void) { 
    render_edit_string(lineRegion, preset_name(*pageSelect), PRESET_NAME_LEN, cursor);
    render_to_scroll_line(SCROLL_CENTER_LINE, 0);
    //   render_scroll_apply_hl(SCROLL_CENTER_LINE, 1);
}
