/*
  menu.c
  bees
  aleph
*/

#include "ui.h"
#include "net.h"
#include "op.h"
#include "key_handler.h"
#include "redraw.h"
#include "menu.h"
#include "menu_protected.h"

//--------------------------
//--------- variables
// const array of user-creatable operator type id's
const opId_t userOpTypes[NUM_USER_OP_TYPES] = {
  eOpAdd,
  eOpMul,
  eOpGate,
  eOpAccum,
  eOpSelect,
  eOpMapLin
};

opId_t newOpType;


// page structures - synchronize with ePage enum
page_t pages[ePageMax] = {
  { "OPS", (keyHandler_t)&key_handler_ops, (redraw_t)&redraw_ops, 0 },
  { "INS", (keyHandler_t)&key_handler_ins, (redraw_t)&redraw_ins, 0 },
  { "OUTS", (keyHandler_t)&key_handler_outs, (redraw_t)&redraw_outs, 0 },
  { "GATHERED" , (keyHandler_t)&key_handler_gathered, (redraw_t)&redraw_gathered, 0 }
};

///// random/ugly
// pointer to current page
page_t* page;
// idx of current page
s8 pageIdx = 0;

// array of onode pointers for gathering
u32(*gathered)[NET_OUTS_MAX];
// how many gathered
u32 numGathered;


//-----------------------------------
//----- external function definitions

// init
extern void menu_init(void) {
  setPage(0);
}

// de-init
extern void menu_deinit(void) {
}

// top level key handler
void menu_handleKey(key_t key) {
  if (key == eKeyEdit) {
    // flip edit/play mode...
  } else {
    page->keyHandler(key);
  }
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
void scrollPage(S8 dir) {
  switch(pageIdx) {
  case ePageOps:
    if (dir > 0) {
      pageIdx = ePageIns;
    } else {
      pageIdx = ePageOuts;
    }
    break;
  case ePageIns:
    if (dir > 0) {
      pageIdx = ePageOuts;
    } else {
      pageIdx = ePageOps;
    }
    break;
  case ePageOuts:
    if (dir > 0) {
      pageIdx = ePageOps;
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
  }
  setPage(pageIdx);
}

// scroll current page selection
void scrollSelect(S8 dir, U32 max) {
  page->selected += dir;
  if (page->selected < 0) {
    page->selected = 0;
  }
  if (page->selected > max) { page->selected = max; }
  // redraw with the new selection
  page->redraw();
}

//////////////////////////////////
///////////////////
///// page specific functions

