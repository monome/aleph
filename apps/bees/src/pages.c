/*
  pages.c
  bees
  aleph
*/

#ifdef ARCH_AVR32
#include "print_funcs.h"
#include "encoders.h"
#endif

#include "pages.h"
//#include "pages_protected.h"
#include "net.h"
#include "op.h"
#include "pages.h"
#include "param.h"
#include "render.h"
#include "screen.h"
#include "ui.h"

// encoder sensitivities
#define ENC_THRESH_PAGESCROLL 32
#define ENC_THRESH_LISTSCROLL 4
#define ENC_THRESH_FINE 0
#define ENC_THRESH_COARSE 4

//--------------------------
//--------- variables

// page structures - synchronize with ePage enum in pages.h
page_t pages[NUM_PAGES] = {
  { .name = "INS",
    .select_fn = &select_ins, // select function
    //    .handler = handler_ins,  // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "OUTS",
    .select_fn = &select_outs, // select function
    //    .handler = handler_outs,  // pointer to handlers
    .encSens = { 4, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "PRESETS",
    .select_fn = &select_presets, // select function
    //    .handler = handler_presets, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "OPS",
    .select_fn = &select_ops, // select function
    //    .handler = handler_ops, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "SCENES",
    .select_fn = &select_scenes, // select function
    //    .handler = handler_scenes, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "DSP",
    .select_fn = &select_dsp, // select function
    //    .handler = handler_dsp, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  // modal:
  { .name = "GATHERED", 
    .select_fn = &select_gathered, // select function
    //    .handler = handler_gathered, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "PLAY",
    .select_fn = &select_play, // select function
    //    .handler = handler_play, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  }
};

// pointer to current page
page_t* curPage;
// idx of current page
s8 pageIdx;
// last pressed key
u8 keyPressed = 255;
// alt-mode flag
u8 altMode = 0;

//--------------
//--- static vars
// saved idx of last non-play page
static s8 lastPageIdx = 0;

//-----------------------------------
//----- external function definitions

// init
 void pages_init(void) {
  init_page_ins();
  init_page_dsp();
  init_page_ops();  
  init_page_outs();
  init_page_play();

  /*
    // TODO
  init_page_presets();
  init_page_scenes();
  init_page_gathered();
  */
  pageIdx = ePageIns;
  set_page(pageIdx);
}

// de-init
 void pages_deinit(void) {
}

// re-select
void pages_reselect(void) {
  curPage->select_fn();
}

// set current page
void set_page(ePage n) {
  u8 i;
  pageIdx = n;
  
  curPage = &(pages[pageIdx]);
  curPage->select_fn();

  //  print_dbg("\r\n set enc sense");
  // set encoder sensitivity
  for(i=0; i<4; i++) {
    //// FIXME: use encoder map?
    //set_enc_thresh(encMap[i], curPage->encSens[i]);
    set_enc_thresh(i, curPage->encSens[i]);
  }
  //  print_dbg("\r\n ...done setting page.");
}

// check key against last pressed
u8 check_key(u8 key) {
  u8 ret;
  if(keyPressed == key) {
    keyPressed = 255;
    ret = 1;
  } else {
    keyPressed = key;
    ret = 0;
  }
  return ret;
}

// toggle play mode
u8 pages_toggle_play(void) {
  u8 ret;
  if(pageIdx == ePagePlay) {
    ret = 0;
    set_page(lastPageIdx);
  } else {
    ret = 1;
    lastPageIdx = pageIdx;
    set_page(ePagePlay);
  }
  return ret;
}
