/*
  menu.c
  bees
  aleph
*/

#include "ui.h"
#include "net.h"
#include "menu.h"

//-----------------------------------
//----- static function declarations

// set current page
static void setPage(ePage n);
// scroll page
static void scrollPage(S8 dir);
// scroll selection in page
static void scrollSelect(S8 dir, U32 max);

// find and print inputs to selected parameter
// static void gatherInputs(void);

//// page-specific key handlers
static void keyHandlerOps(key_t key);
static void keyHandlerIns(key_t key);
static void keyHandlerOuts(key_t key);

//// page-specific redraws
static void redrawOps(void);
static void redrawIns(void);
static void redrawOuts(void);

//-----------------------------------
//------- static variables

// page titles - synchronize with ePage enum
static page_t pages[ePageMax] = {
  { "OPS",    (keyHandler_t)&keyHandlerOps,    (redraw_t)&redrawOps, 0 },
  { "INS",    (keyHandler_t)&keyHandlerIns,    (redraw_t)&redrawIns, 0 },
  { "OUTS",   (keyHandler_t)&keyHandlerOuts,    (redraw_t)&redrawOuts, 0 }
};

// pointer to current page
static page_t* page;
// idx of current page
static u8 pageIdx = 0;

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
  page->keyHandler(key);
}

//-----------------------------------
//----- static function definitions

// set current page
static void setPage(ePage n) {
  pageIdx = n;
  page = &(pages[pageIdx]);
  page->redraw();
}

// scroll current page
static void scrollPage(S8 dir) {
  pageIdx += dir;
  if (pageIdx < 0) {
    pageIdx = 0;
  }
  if (pageIdx >= ePageMax) { pageIdx = 0; }
  setPage(pageIdx);
}


// scroll current page selection
static void scrollSelect(S8 dir, U32 max) {
  page->selected += dir;
  if (page->selected < 0) {
    page->selected = 0;
  }
  if (page->selected > max) { page->selected = max; }
  // redraw with the new selection
  page->redraw();
}


/*
// find and print connected output nodes
static void gatherInputs(void) {
}
*/

//========================================
//====== key handlers
// OPS
void keyHandlerOps(key_t key) {
  switch(key) {
  case eKeyFnA:
    break;
  case eKeyFnB:
    break;
  case eKeyFnC:
    break;
  case eKeyFnD:
    break;
    //// encoder A: scroll pages
  case eKeyUpA:
    scrollPage(1);
    break;
  case eKeyDownA:
    scrollPage(-1);
    break;
    //// encoder B: scroll selection
  case eKeyUpB:
    scrollSelect(1, net_num_ops());
    break;
  case eKeyDownB:
    scrollSelect(-1, net_num_ops());      
    break;
  case eKeyUpC:
    // nothing
    break;
  case eKeyDownC:
    // nothing
    break;
  case eKeyUpD:
    // nothing
    break;
  case eKeyDownD:
    // nothing
    break;
  case eKeyMax: // dummy
    // nothing
    break;
  default:
    ;; // nothing
  }  
}

// INS
void keyHandlerIns(key_t key) {
  switch(key) {
  case eKeyFnA:
    break;
  case eKeyFnB:
    break;
  case eKeyFnC:
    break;
  case eKeyFnD:
    break;
    //// encoder A: scroll pages
  case eKeyUpA:
    scrollPage(1);
    break;
  case eKeyDownA:
    scrollPage(-1);
    break;
    //// encoder B: scroll selection
  case eKeyUpB:
    scrollSelect(1, net_num_ops());
    break;
  case eKeyDownB:
    scrollSelect(-1, net_num_ops());      
    break;
  case eKeyUpC:
    // nothing
    break;
  case eKeyDownC:
    // nothing
    break;
  case eKeyUpD:
    // nothing
    break;
  case eKeyDownD:
    // nothing
    break;
  case eKeyMax: // dummy
    // nothing
    break;
  default:
    ;; // nothing
  }  
}

// OUTS
void keyHandlerOuts(key_t key) {
  switch(key) {
  case eKeyFnA:
    break;
  case eKeyFnB:
    break;
  case eKeyFnC:
    break;
  case eKeyFnD:
    break;
    //// encoder A: scroll pages
  case eKeyUpA:
    scrollPage(1);
    break;
  case eKeyDownA:
    scrollPage(-1);
    break;
    //// encoder B: scroll selection
  case eKeyUpB:
    scrollSelect(1, net_num_ops());
    break;
  case eKeyDownB:
    scrollSelect(-1, net_num_ops());      
    break;
  case eKeyUpC:
    // nothing
    break;
  case eKeyDownC:
    // nothing
    break;
  case eKeyUpD:
    // nothing
    break;
  case eKeyDownD:
    // nothing
    break;
  case eKeyMax: // dummy
    // nothing
    break;
  default:
    ;; // nothing
  }  
}

//========================================
//======= redraws

// redraw ops page
extern void redrawOps(void) {
  U8 y = 0;                       // which line
  S32 n = page->selected;         // which list entry
  const U16 num = net_num_ops(); // how many
  const U16 num_1 = num - 1;

  // draw the header
  ui_print(y, 0, "__OPS____");

  // deal with empty list
  if (num<1) { 
  } else {
    //// draw the ops list...
    // draw selection at center
    y = SCREEN_ROW_CENTER;
    ui_print(y, 0, ">");
    ui_print(y, 1, net_op_name(n));
    // draw higher entries if they exist
    n++;
    y++;
    while ((y<SCREEN_H_1) && (n < num_1)) {
      ui_print(y, 1, net_op_name(n));
      n++;      
      y++;
    } 
    // draw lower entries if they exist
    n = page->selected - 1;
    y = SCREEN_ROW_CENTER - 1;
    while ((y >= 1) && (n >= 0)) {
      ui_print(y, 1, net_op_name(n));
      n--;
      y--;
    }
  } 
  // draw footer (function labels)
}

// redraw inputs page
extern void redrawIns(void) {
  // draw the header
  ui_println(0, "__INS____");
}

// redraw outputs page
extern void redrawOuts(void) {
  // draw the header
  ui_println(0, "__OUTS___");
}
