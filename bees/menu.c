/*
  menu.c
  bees
  aleph
*/

#include "key_handler.h"
#include "menu.h"
#include "menu_protected.h"
#include "net.h"
#include "op.h"
#include "param.h"
#include "redraw.h"
#include "screen.h"
#include "ui.h"

//--------------------------
//--------- variables
// const array of user-creatable operator type id's
const opId_t userOpTypes[NUM_USER_OP_TYPES] = {
  eOpAdd,
  eOpMul,
  eOpGate,
  // eOpAccum,
  // eOpSelect,
  // eOpMapLin
};

// page structures - synchronize with ePage enum
page_t pages[ePageMax] = {
  // list:
  { "INS", (keyHandler_t)&key_handler_ins, (redraw_t)&redraw_ins, 0, eModeNone, -1 },
  { "OUTS", (keyHandler_t)&key_handler_outs, (redraw_t)&redraw_outs, 0, eModeNone, -1 },
  { "PRESETS", (keyHandler_t)&key_handler_presets, (redraw_t)&redraw_presets, 0, eModeNone, -1 },
  { "OPS", (keyHandler_t)&key_handler_ops, (redraw_t)&redraw_ops, 0, eModeNone, -1 },
  { "SCENES", (keyHandler_t)&key_handler_scenes, (redraw_t)&redraw_scenes, 0, eModeNone, -1 },
  { "DSP", (keyHandler_t)&key_handler_dsp, (redraw_t)&redraw_dsp, 0, eModeNone, -1 },
  // modal:
  { "GATHERED" , (keyHandler_t)&key_handler_gathered, (redraw_t)&redraw_gathered, 0, eModeNone, -1 },
  { "PLAY" , (keyHandler_t)&key_handler_play, (redraw_t)&redraw_play, 0, eModeNone, -1}
};

// pointer to current page
page_t* page;
// idx of current page
s8 pageIdx = 0;
// new operator type
opId_t newOpType;
// array of onode pointers for gathering
u32(*gathered)[NET_OUTS_MAX];
// how many gathered
u32 numGathered;
// last touchede parameter indices
touched_t touchedParams[SCREEN_H];

//-----------------------
//------ static vars
// saved idx for toggling in play mode
static s8 savedPageIdx = 0;

//-----------------------------------
//----- external function definitions

// init
extern void menu_init(void) {
  setPage(pageIdx);
}

// de-init
extern void menu_deinit(void) {
}

// top level key handler
void menu_handleKey(uiKey_t key) {
  if (key == eKeyEdit) {
    if (pageIdx == ePagePlay) {
      // restore saved page
      setPage(savedPageIdx);
    } else {
      // save the page and switch to Play mode
      savedPageIdx = pageIdx;
      setPage(ePagePlay);
    }
  } else {
    page->keyHandler(key);
  }
#if ARCH_LINUX
  screen_refresh();
#endif
}


//-----------------------------------
//----- static function definitions

// set current page
void setPage(ePage n) {
  pageIdx = n;
  page = &(pages[pageIdx]);
  page->redraw();
}

// scroll current page
void scrollPage(s8 dir) {
  switch(pageIdx) {
  case ePageIns:
    if (dir > 0) {
      pageIdx = ePageOuts;
    } else {
      pageIdx = ePageOps;
    }
    break;
  case ePageOuts:
    if (dir > 0) {
      pageIdx = ePagePresets;
    } else {
      pageIdx = ePageIns;
    }
    break;
  case ePageGathered:
    if (dir > 0) {
      pageIdx = ePageIns;
    } else {
      pageIdx = ePageOuts;
    }
    break;
  case ePagePresets:
    if (dir > 0) {
      pageIdx = ePageOps;
    } else {
      pageIdx = ePageOuts;
    }
    break;

  case ePageOps:
    if (dir > 0) {
      pageIdx = ePageScenes;
    } else {
      pageIdx = ePagePresets;
    }
    break;
  case ePageScenes:
    if (dir > 0) {
      pageIdx = ePageDsp;
    } else {
      pageIdx = ePageOps;
    }
    break;
  case ePageDsp:
    if (dir > 0) {
      pageIdx = ePageIns;
    } else {
      pageIdx = ePageScenes;
    }
    break;
  }
  setPage(pageIdx);
}

// scroll current page selection
void scrollSelect(s8 dir, s32 max) {
  page->selected += dir;
  while (page->selected < 0) {
    page->selected += max;
      //    page->selected = 0;
  }
  while (page->selected >= max) {
    // page->selected = max;
    page->selected -= max;
  }
  // redraw with the new selection
  page->redraw();
}

// parameter feedback
void param_feedback(u16 paramIdx) {
  int i;
  for (i = 1; i < SCREEN_H; i++) {
    touchedParams[i-1].idx = touchedParams[i].idx;
    touchedParams[i-1].val = touchedParams[i].val;
  }
  touchedParams[SCREEN_H_1].idx = paramIdx;
  touchedParams[SCREEN_H_1].val = get_param_value(paramIdx );

}
