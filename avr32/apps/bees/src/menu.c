/*
  menu.c
  bees
  aleph
*/

#ifdef ARCH_AVR32
#include "print_funcs.h"
#include "encoders.h"
#endif

#include "key_handler.h"
#include "menu.h"
#include "menu_protected.h"
#include "net.h"
#include "op.h"
#include "param.h"
#include "redraw.h"
#include "screen.h"
#include "ui.h"

// encoder sensitivities
#define ENC_THRESH_PAGESCROLL 32
#define ENC_THRESH_LISTSCROLL 4
#define ENC_THRESH_FINE 0
#define ENC_THRESH_COARSE 4

//--------------------------
//--------- variables
// const array of user-creatable operator type id's
/// FIXME: this is dumb
const op_id_t userOpTypes[NUM_USER_OP_TYPES] = {
  eOpAdd,
  eOpMul,
  eOpGate,
  /// more...
  // eOpAccum,
  // eOpSelect,
};

// page structures - synchronize with ePage enum in menu.h
page_t pages[NUM_PAGES] = {
  // list:
  { "INS", (keyHandler_t)&key_handler_ins, (redraw_t)&redraw_ins, 0, eModeNone, -1, 
    { ENC_THRESH_PAGESCROLL,  ENC_THRESH_LISTSCROLL, 0, 0 } },
  
  { "OUTS", (keyHandler_t)&key_handler_outs, (redraw_t)&redraw_outs, 0, eModeNone, -1, 
    { ENC_THRESH_PAGESCROLL,  ENC_THRESH_LISTSCROLL, 4, 0 }  },
  
  {  "PRESETS", (keyHandler_t)&key_handler_presets, (redraw_t)&redraw_presets, 0, eModeNone, -1,
    { ENC_THRESH_PAGESCROLL,  ENC_THRESH_LISTSCROLL, 4, 4 } },
  
  { "OPS", (keyHandler_t)&key_handler_ops, (redraw_t)&redraw_ops, 0, eModeNone, -1, 
    { ENC_THRESH_PAGESCROLL,  ENC_THRESH_LISTSCROLL, 4, 4 } },

  { "SCENES", (keyHandler_t)&key_handler_scenes, (redraw_t)&redraw_scenes, 0, eModeNone, -1, 
    { ENC_THRESH_PAGESCROLL,  ENC_THRESH_LISTSCROLL, 4, 4 } },

  { "DSP", (keyHandler_t)&key_handler_dsp, (redraw_t)&redraw_dsp, 0, eModeNone, -1, 
    { ENC_THRESH_PAGESCROLL,  ENC_THRESH_LISTSCROLL, 4, 4 } },

  // modal:
  { "GATHERED" , (keyHandler_t)&key_handler_gathered, (redraw_t)&redraw_gathered, 0, eModeNone, -1, 
    { ENC_THRESH_PAGESCROLL,  ENC_THRESH_LISTSCROLL, 4, 0 } },

  { "PLAY" , (keyHandler_t)&key_handler_play, (redraw_t)&redraw_play, 0, eModeNone, -1, 
    { 0, 0, 0, 0 } }
};

// pointer to current page
page_t* curPage;
// idx of current page
s8 pageIdx = 0;
// new operator type
op_id_t newOpType;
// array of onode pointers for gathering
u32(*gathered)[NET_OUTS_MAX];
// how many gathered
u32 numGathered;

//-----------------------
//------ static vars
// saved idx for toggling in play mode
static s8 savedPageIdx = 0;

// const array to re-map hardware encoders (sorry...)
static const u8 encMap[] = { 3, 2, 1, 0 };

//-----------------------------------
//----- external function definitions

// init
extern void menu_init(void) {
  set_page(pageIdx);
}

// de-init
extern void menu_deinit(void) {
}

// top level key handler
void menu_handleKey(uiKey_t key, s16 val) {
  if (key == eKeyMode) {
    if (pageIdx == ePagePlay) {
      // restore saved page
      set_page(savedPageIdx);
    } else {
      // save the page and switch to Play mode
      savedPageIdx = pageIdx;
      set_page(ePagePlay);
    }
  } else {
    //    print_dbg("\r\n menu_handleKey");
    curPage->keyHandler(key, val);
    
  }
#if ARCH_LINUX
  screen_refresh();
#endif
}

// refresh
extern void menu_refresh(void) {
  curPage->redraw();
}

// set current page
void set_page(ePage n) {
  u8 i;
  pageIdx = n;
  //  print_dbg("\r\n page set");
  curPage = &(pages[pageIdx]);
  //  print_dbg("\r\n redraw after page set:");
  curPage->redraw();

#if ARCH_AVR32
  //  print_dbg("\r\n set enc sense");
  // set encoder sensitivity
  for(i=0; i<4; i++) {
    set_enc_thresh(encMap[i], curPage->encSens[i]);
  }
#endif
}

//// ins -> outs -> (gathered) -> presets -> scenes -> dsp
// scroll current page
void scroll_page(s8 dir) {
  //  print_dbg("\r\nscroll page: ");
  //  print_dbg_hex(pageIdx);
  switch(pageIdx) {
  case ePageIns:
    pageIdx = dir>0 ? ePageOuts : ePageDsp ;
    break;
  case ePageOuts:
    pageIdx = dir>0 ? ePagePresets : ePageIns ;
    break;
  case ePageGathered:
    pageIdx = dir>0 ? ePagePresets : ePageIns ;
    break;
  case ePagePresets:
    pageIdx = dir>0 ? ePageOps : ePageOuts ;
    break;
  case ePageOps:
    pageIdx = dir>0 ? ePageScenes : ePagePresets ;
    break;
  case ePageScenes:
    pageIdx = dir>0 ? ePageDsp : ePageOps ;
    break;
  case ePageDsp:
    pageIdx = dir>0 ? ePageIns : ePageScenes ;
    break;
  }
  //  print_dbg("\r\n new page idx: ");
  //  print_dbg_hex(pageIdx);
  set_page(pageIdx);
}



// scroll current page selection
//--  clipping variant
void scroll_select_clip(s8 dir, s32 max) {
  curPage->selected += dir;
  //  print_dbg("\r\n curPage->selected: ");
  //  print_dbg_hex(curPage->selected);

  if(curPage->selected >= max) {
    curPage->selected = max - 1;
  }
  if(curPage->selected < 0) {
    curPage->selected = 0;
  }
  curPage->redraw();
}

//--  wrapping variant
void scroll_select_wrap(s8 dir, s32 max) {
  curPage->selected += dir;
    while (curPage->selected < 0) {
      curPage->selected += max;
    }
    while (curPage->selected >= max) {
      curPage->selected -= max; 
    }
  curPage->redraw();
}

//-- default: wrap
void scroll_select(s8 dir, s32 max) {
  scroll_select_wrap(dir, max);
}


// get selection on given page
extern s16 menu_selection(ePage page) {
  return pages[page].selected;
}

// display system notification
extern void menu_notify(const char* str) {
  screen_blank();
  screen_line(0, 0, str, 0xf);
}
