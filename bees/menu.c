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
const opId_t userOpTypes[NUM_USER_OP_TYPES] = {
  eOpAdd,
  eOpMul,
  eOpGate,
  // eOpAccum,
  // eOpSelect,
};

// page structures - synchronize with ePage enum
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
opId_t newOpType;
// array of onode pointers for gathering
u32(*gathered)[NET_OUTS_MAX];
// how many gathered
u32 numGathered;
// last touchede parameter indices
touched_t touchedParams[CHAR_ROWS];

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
    curPage->keyHandler(key);
  }
#if ARCH_LINUX
  screen_refresh();
#endif
}

//-----------------------------------
//----- static function definitions

// set current page
void set_page(ePage n) {
  u8 i;
  pageIdx = n;
  curPage = &(pages[pageIdx]);
  curPage->redraw();

#if ARCH_AVR32
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
void scroll_select(s8 dir, s32 max) {
  //  print_dbg("\r\nSCROLL SELECT");
  curPage->selected += dir;
  while (curPage->selected < 0) {
    curPage->selected += max;
      //    page->selected = 0;
  }
  while (curPage->selected >= max) {
    // page->selected = max;
    curPage->selected -= max;
  }
  // redraw with the new selection
  curPage->redraw();
}

// parameter feedback
void param_feedback(u16 paramIdx, fix16 val) {
  int i;
  print_dbg("\r\n param fb: ");
  print_dbg_hex(paramIdx);
  for (i = 1; i < CHAR_ROWS; i++) {
    touchedParams[i-1].idx = touchedParams[i].idx;
    touchedParams[i-1].val = touchedParams[i].val;
  }
  touchedParams[CHAR_ROWS_1].idx = paramIdx;
  touchedParams[CHAR_ROWS_1].val = val; //get_param_value(paramIdx );
}

