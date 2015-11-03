/*
  page_scenes.c
 */

#include "print_funcs.h"

#include "files.h"
#include "handler.h"
#include "net.h"
#include "pages.h"
#include "render.h"
#include "scene.h"

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
static s8 cursor = 0;

// kludge:
// constant pointer to this page's selection
static s16* const pageSelect = &(pages[ePageScenes].select);

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

// fill tmp region with new content
// given input index and foreground color
static void render_line(s16 idx, u8 fg) {
  region_fill(lineRegion, 0x0);
  if( (idx >= 0) && (idx < files_get_scene_count()) ) {
    clearln();
    appendln((const char*)files_get_scene_name(idx));
    // stick a null character at the end...
    lineBuf[SCENE_NAME_LEN - 1] = '\0';
    font_string_region_clip(lineRegion, lineBuf, 2, 0, fg, 0);
  }
}

// scroll the current selection
static void select_scroll(s32 dir) {
  const s32 max = files_get_scene_count() -1;
  // index for new content
  s16 newIdx;
  s16 newSel;

  // cancel actions
  pages_reset_keypressed();
  cursor = 0;

  if(dir < 0) {
    /// SCROLL DOWN
    if(*pageSelect == 0) {
      return;
    }
    // remove highlight from old center
    render_scroll_apply_hl(SCROLL_CENTER_LINE, 0);
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
    if(*pageSelect == (max) ) {
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
  if(val == 0) { return; }
  if(check_key(0)) {
    region_fill(headRegion, 0x0);
    font_string_region_clip(headRegion, "writing scene to card...", 0, 0, 0xa, 0);
    headRegion->dirty = 1;
    render_update();
    region_fill(headRegion, 0x0);


    //    files_store_scene_name(sceneData->desc.sceneName, 1);
    files_store_scene_name(sceneData->desc.sceneName);

    print_dbg("\r\n stored scene, back to handler");
    
    font_string_region_clip(headRegion, "done writing.", 0, 0, 0xa, 0);
    headRegion->dirty = 1;
    render_update();

    // refresh
    //    redraw_lines();
    redraw_scenes();
  }
  show_foot();
}

// recall
void handle_key_1(s32 val) {
  if(val == 1) { return; }
  if(check_key(1)) {
    notify("reading...");
    files_load_scene(*pageSelect);
    redraw_ins();
    redraw_outs();
    redraw_ops();
    redraw_scenes();
    notify("done reading.");
  }
  show_foot();
}

void handle_key_2(s32 val) {
  // clear
  if(val == 1) { return; }
  if(check_key(2)) {
    notify("clearing...");
    net_clear_user_ops();
    redraw_ins();
    redraw_outs();
    redraw_ops();
    redraw_scenes();
    notify("done clearing.");
  }
  show_foot();
}

void handle_key_3(s32 val) {
  if ( pages_set_alt(val)) { 
    sceneData->desc.sceneName[cursor] = '\0';
    redraw_scenes();
    render_edit_string(headRegion, sceneData->desc.sceneName, SCENE_NAME_LEN, cursor);
  }
  show_foot();
}

// scroll character value at cursor position in scene name
void handle_enc_3(s32 val) {
  if(val > 0) {
    pages_edit_char_inc(sceneData->desc.sceneName, cursor);
  } else {
    pages_edit_char_dec(sceneData->desc.sceneName, cursor);
  }
  print_dbg("\r\b edited scene name: ");
  print_dbg(sceneData->desc.sceneName);
  render_edit_string(headRegion, sceneData->desc.sceneName, SCENE_NAME_LEN, cursor);
}

// scroll cursor position in current scene name
void handle_enc_2(s32 val) {
  pages_edit_cursor(val, sceneData->desc.sceneName, &cursor, SCENE_NAME_LEN);
  /*
  if(val > 0) {
    ++cursor;
    if (cursor >= SCENE_NAME_LEN) {
      cursor = SCENE_NAME_LEN - 1;
    } 
  } else {
    --cursor;
    if (cursor < 0) {
      cursor = 0;
    } 
  }

  if(altMode) { 
    sceneData->desc.sceneName[cursor] = '\0';
  } else {
    if(sceneData->desc.sceneName[cursor] == '\0') { 
      sceneData->desc.sceneName[cursor] = '_';
      // also search for other nulls? eh, could end with deleted text
    }
  }
  */
  render_edit_string(headRegion, sceneData->desc.sceneName, SCENE_NAME_LEN, cursor);
}


// enc 0 : scroll page
void handle_enc_1(s32 val) {
   if(val > 0) {
    set_page(ePageDsp);
  } else {
    set_page(ePagePresets);
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
void init_page_scenes(void) {
  u8 i, n;
  print_dbg("\r\n alloc SCENES page");
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
void select_scenes(void) {
  //  print_dbg("\r\n select SCENES... ");
  // assign global scroll region pointer
  // also marks dirty
  //  print_dbg("\r\n set scroll region...");
  render_set_scroll(&centerScroll);
  //  print_dbg("\r\n fill head region...");
  // other regions are static in top-level render, with global handles
  region_fill(headRegion, 0x0);
  font_string_region_clip(headRegion, "SCENES", 0, 0, 0xf, 0x1);
  show_foot();
  // assign handlers
  //  print_dbg("\r\n assign page handlers.....");
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
void redraw_scenes(void) {
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
