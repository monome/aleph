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
// const array of user-creatable operator type id's
/// FIXME: this is dumb, should somehow be specified in op.c or similar
/*
const op_id_t userOpTypes[NUM_USER_OP_TYPES] = {
  eOpAdd,
  eOpMul,
  eOpGate,
  eOpMonomeGridRaw
  // many, many more...
  /// more...
  // eOpAccum,
  // eOpSelect,
};
*/

// page structures - synchronize with ePage enum in pages.h
page_t pages[NUM_PAGES] = {
  { .name = "INS",
    .refresh = &refresh_ins,
    .handler = handler_ins, // pointer to handlers
    .encSens = { 0, 0, 0, 0, }, // encoder sens 
  },
  { .name = "OUTS",
    .refresh = &refresh_outs,
    .handler = handler_outs, // pointer to handlers
    .encSens = { ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, 4, 0, }, // encoder sens 
  },
  { .name = "PRESETS",
    .refresh = &refresh_presets,
    .handler = handler_presets, // pointer to handlers
    .encSens = { ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, 4, 4, }, // encoder sens 
  },
  { .name = "OPS",
    .refresh = &refresh_dsp,
    .handler = handler_dsp, // pointer to handlers
    .encSens = { ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, 4, 4, }, // encoder sens 
  },
  { .name = "SCENES",
    .refresh = &refresh_scenes,
    .handler = handler_scenes, // pointer to handlers
    .encSens = { ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, 4, 4, }, // encoder sens 
  },
  { .name = "DSP",
    .refresh = &refresh_dsp,
    .handler = handler_dsp, // pointer to handlers
    .encSens = { ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, 4, 4, }, // encoder sens 
  },
  // modal:
  { .name = "GATHERED",
    .refresh = &refresh_gathered,
    .handler = handler_gathered, // pointer to handlers
    .encSens = { ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, 4, 0, }, // encoder sens 
  },
  { .name = "PLAY",
    .refresh = &refresh_play,
    .handler = handler_play, // pointer to handlers
    .encSens = { ENC_THRESH_PAGESCROLL, ENC_THRESH_LISTSCROLL, 0, 0, }, // encoder sens 
  }
};

// pointer to current page
page_t* curPage;
// idx of current page
s8 pageIdx = 0;

/*
// new operator type
op_id_t newOpType;
// array of onode pointers for gathering
u32(*gathered)[NET_OUTS_MAX];
// how many gathered
u32 numGathered;
*/

//-----------------------
//------ static vars
// saved idx for toggling in play mode
// static s8 savedPageIdx = 0;

//-----------------------------------
//----- external function definitions

// init
extern void pages_init(void) {
  init_page_ins();
  /*
    // TODO
  init_page_outs();
  init_page_presets();
  init_page_ops();
  init_page_scenes();
  init_page_dsp();
  init_page_gathered();
  init_page_play();
  */
  set_page(pageIdx);
}

// de-init
extern void pages_deinit(void) {
}

// refresh

extern void pages_refresh(void) {
  curPage->refresh();
}

// set current page
void set_page(ePage n) {
  u8 i;
  pageIdx = n;
  
  print_dbg("\r\n setting page... ");

  //  print_dbg("\r\n page set");
  curPage = &(pages[pageIdx]);
  //  print_dbg("\r\n render after page set:");
  //  curPage->select();


  print_dbg("\r\n refreshing page... ");
  //  print_dbg(" ( not really ... refresh FP is 0x");
  //  print_dbg_hex((u32)(curPage->refresh));
  curPage->refresh();

  print_dbg("\r\n set enc sense");
  // set encoder sensetivity
  for(i=0; i<4; i++) {
    //set_enc_thresh(encMap[i], curPage->encSens[i]);
    set_enc_thresh(i, curPage->encSens[i]);
  }
}
