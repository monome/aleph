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
    .refresh = &refresh_ins, // refresh function
    .handler = handler_ins,  // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "OUTS",
    .refresh = &refresh_outs, // refresh function
    .handler = handler_outs,  // pointer to handlers
    .encSens = { 4, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "PRESETS",
    .refresh = &refresh_presets, // refresh function
    .handler = handler_presets, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "OPS",
    .refresh = &refresh_ops, // refresh function
    .handler = handler_ops, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "SCENES",
    .refresh = &refresh_scenes, // refresh function
    .handler = handler_scenes, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "DSP",
    .refresh = &refresh_dsp, // refresh function
    .handler = handler_dsp, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  // modal:
  { .name = "GATHERED", 
    .refresh = &refresh_gathered, // refresh function
    .handler = handler_gathered, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  },
  { .name = "PLAY",
    .refresh = &refresh_play, // refresh function
    .handler = handler_play, // pointer to handlers
    .encSens = { 0, 0, ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, }, // encoder sens 
  }
};

// pointer to current page
page_t* curPage;
// idx of current page
s8 pageIdx;
// last pressed key
u8 keyPressed = 255;

//--------------
//--- static vars
// saved idx of last non-play page
static s8 lastPageIdx = 0;

//-----------------------------------
//----- external function definitions

// init
 void pages_init(void) {
  init_page_ins();
  init_page_outs();
  init_page_dsp();
  init_page_ops();
  /*
    // TODO
  init_page_presets();
  init_page_scenes();
  init_page_gathered();
  init_page_play();
  */
  pageIdx = ePageIns;
  set_page(pageIdx);
}

// de-init
 void pages_deinit(void) {
}

// refresh

void pages_refresh(void) {
  curPage->refresh();
}

// set current page
void set_page(ePage n) {
  u8 i;
  pageIdx = n;
  
  print_dbg("\r\n setting page, idx: ");
  print_dbg_ulong(n);
  curPage = &(pages[pageIdx]);

  print_dbg("\r\n refreshing page... ");
  print_dbg(" refresh FP is 0x");
  print_dbg_hex((u32)(curPage->refresh));
  curPage->refresh();

  print_dbg("\r\n set enc sense");
  // set encoder sensitivity
  for(i=0; i<4; i++) {
    //// FIXME: use encoder map?
    //set_enc_thresh(encMap[i], curPage->encSens[i]);
    set_enc_thresh(i, curPage->encSens[i]);
  }
  print_dbg("\r\n ...done setting page.");
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
