/*
  menu.c
  bees
  aleph
*/

#ifdef ARCH_AVR32
// #include "print_funcs.h"
#include "encoders.h"
#endif

#include "key_handler.h"
#include "menu.h"
#include "menu_protected.h"
#include "redraw.h"
#include "screen.h"
//#include "ui.h"

// encoder sensitivities
#define ENC_THRESH_PAGESCROLL 32
#define ENC_THRESH_LISTSCROLL 4
#define ENC_THRESH_FINE 0
#define ENC_THRESH_COARSE 4

//--------------------------
//--------- variables

// page structures - synchronize with ePage enum
page_t pages[NUM_PAGES] = {
  // list:
  { "DSP", (keyHandler_t)&key_handler_dsp, (redraw_t)&redraw_dsp, 0, eModeNone, -1, 
    { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL, 4, 4 } },

  { "FIRMWARE", (keyHandler_t)&key_handler_fw, (redraw_t)&redraw_fw, 0, eModeNone, -1, 
    { ENC_THRESH_LISTSCROLL, ENC_THRESH_PAGESCROLL, 4, 4 } },
};

// pointer to current page
page_t* curPage;
// idx of current page
s8 pageIdx = 1;

//-----------------------
//------ static vars
// saved idx for toggling in play mode
// static s8 savedPageIdx = 0;

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
  /*   if (pageIdx == ePagePlay) { */
  /*     // restore saved page */
  /*     set_page(savedPageIdx); */
  /*   } else { */
  /*     // save the page and switch to Play mode */
  /*     savedPageIdx = pageIdx; */
  /*     set_page(ePagePlay); */
  /*   } */
  }
  curPage->keyHandler(key);
  
#if ARCH_LINUX
  screen_refresh();
#endif
}

// refresh gfx
extern void menu_refresh(void) {
  curPage->redraw();
  screen_refresh();
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
  switch(pageIdx) {
  case ePageDsp:
    pageIdx = ePageFw;
    break;
  case ePageFw:
    pageIdx = ePageDsp;
    break;
  default:
    pageIdx = ePageDsp;
    break;
  }
  set_page(pageIdx);
}

// scroll current page selection
//--  clipping variant
void scroll_select_clip(s8 dir, s32 max) {
  curPage->selected += dir;
  //  // print_dbg("\r\n curPage->selected: ");
  //  // print_dbg_hex(curPage->selected);

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

