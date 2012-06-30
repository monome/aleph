/*
  menu.c
  bees
  aleph
*/

#include "ui.h"
#include "net.h"
#include "op.h"
#include "menu.h"

//-----------------------------------
//----- static function declarations

// set current pages
static void setPage(ePage n);
// scroll page
static void scrollPage(S8 dir);
// scroll selection in page
static void scrollSelect(S8 dir, U32 max);

//// page-specific key handlers
static void keyHandlerOps(key_t key);
static void keyHandlerIns(key_t key);
static void keyHandlerOuts(key_t key);
static void keyHandlerGathered(key_t key);

//// page-specific redraws
static void redrawOps(void);
static void redrawIns(void);
static void redrawOuts(void);
static void redrawGathered(void);

//// line redraws
static void drawOpLine(S32 n, U16 num, U8 y, U8 statMod);
static void drawInLine(S32 n, U16 num, U8 y, U8 statMod);
static void drawOutLine(S32 n, U16 num, U8 y, U8 statMod);

//-----------------------------------
//------- static variables

//---- constants
static const S32 kParamValStepSmall = 1;
static const S32 kParamValStepLarge = 512;

// const array of user-creatable operator type id's
#define NUM_USER_OP_TYPES 6
#define NUM_USER_OP_TYPES_1 5
static const opId_t userOpTypes[NUM_USER_OP_TYPES] = {
  eOpAdd,
  eOpMul,
  eOpGate,
  eOpAccum,
  eOpSelect,
  eOpMapLin
};

// page structures - synchronize with ePage enum
static page_t pages[ePageMax] = {
  { "OPS", (keyHandler_t)&keyHandlerOps, (redraw_t)&redrawOps, 0 },
  { "INS", (keyHandler_t)&keyHandlerIns, (redraw_t)&redrawIns, 0 },
  { "OUTS", (keyHandler_t)&keyHandlerOuts, (redraw_t)&redrawOuts, 0 },
  { "GATHERED" , (keyHandler_t)&keyHandlerGathered, (redraw_t)&redrawGathered, 0 }
};

///// random/ugly
// pointer to current page
static page_t* page;
// idx of current page
static s8 pageIdx = 0;
// new operator class index
static opId_t newOpType = 0;
// array of onode pointers for gathering
static U32(*gathered)[NET_OUTS_MAX];
// how many gathered
static u32 numGathered;
// line buffer
static char lineBuf[SCREEN_W];


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
static void setPage(ePage n) {
  pageIdx = n;
  page = &(pages[pageIdx]);
  page->redraw();
}

// scroll current page
static void scrollPage(S8 dir) {
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
static void scrollSelect(S8 dir, U32 max) {
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

//========================================
//====== key handlers
// OPS
void keyHandlerOps(key_t key) {
  U16 n;
  switch(key) {
  case eKeyFnA: 
    // fnA go to selected operator's inputs on INS page
    pages[ePageIns].selected = net_op_in_idx(page->selected, 0);
    setPage(ePageIns);
    break;
  case eKeyFnB:
    // fnB : go to this operator's outputs on OUTS page
    pages[ePageOuts].selected = net_op_out_idx(page->selected, 0);
    setPage(ePageOuts);
    break;
  case eKeyFnC:
    // fnC : create new operator of specified type
    net_add_op(userOpTypes[newOpType]);
    redrawOps();
    break;
  case eKeyFnD:
    // fnD : delete
    // FIXME: need to add arbitrary op deletion.
    // right now this will destroy the last created op
    if (net_op_status(net_num_ops() - 1) != eUserOp) {
      return;
    }
    net_pop_op();
    n = net_num_ops() - 1;
    if (page->selected > n) {
      page->selected = n;
    }
    redrawOps();
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
    scrollSelect(1, net_num_ops() - 1);
    break;
  case eKeyDownB:
    scrollSelect(-1, net_num_ops() - 1);      
    break;
    //// encoder C: move up/down in order of execution
  case eKeyUpC:
    // TODO
    break;
  case eKeyDownC:
    // TODO
    break;
    //// encoder D: select new operator type for creation
  case eKeyUpD:
    newOpType++;
    if (newOpType >= NUM_USER_OP_TYPES) {
      newOpType = 0;
    }
    redrawOps();
    break;
  case eKeyDownD:
    newOpType--;
    if (newOpType >= NUM_USER_OP_TYPES) {
      newOpType = NUM_USER_OP_TYPES_1;
    }
    redrawOps();
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
  u32 i;
  switch(key) {
  case eKeyFnA:
    // fnA : gather
    numGathered = net_gather(page->selected, gathered);
    break;
  case eKeyFnB:
    // fnB : disconnect
    numGathered = net_gather(page->selected, gathered);
    for(i=0; i<numGathered; i++) {
      net_disconnect(*(gathered[i]));
    }
    break;
  case eKeyFnC:
    // fnC : overwrite preset value (and include)
    // TODO
    break;
  case eKeyFnD:
    // toggle preset inclusion
    net_toggle_preset_in(page->selected);
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
    scrollSelect(1, net_num_ins()-1);
    break;
  case eKeyDownB:
    scrollSelect(-1, net_num_ins()-1);      
    break;
  case eKeyUpC:
    // encoder C : value slow
    net_inc_in_value(page->selected, kParamValStepSmall);
    redrawIns();
    break;
  case eKeyDownC:
    net_inc_in_value(page->selected, kParamValStepSmall * -1);
    redrawIns();
    break;
  case eKeyUpD:
    // encoder D : value fast
    net_inc_in_value(page->selected, kParamValStepLarge);
    redrawIns();
    break;
  case eKeyDownD:
    net_inc_in_value(page->selected, kParamValStepLarge * -1);
    redrawIns();
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
  S16 i;
  static S32 target;
  switch(key) {
  case eKeyFnA: 
    // follow
    i = net_get_target(page->selected);
    if (i == -1) { return; } 
    pages[ePageIns].selected = i;
    setPage(ePageIns);
    break;
  case eKeyFnB:
    // disconnect
    break;
  case eKeyFnC:
    // re-send? store?
    break;
  case eKeyFnD:
    // toggle preset (target)
    i = net_get_target(page->selected);
    if(i>=0) { net_toggle_preset_in(i); }
    redrawOuts();
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
    scrollSelect(1, net_num_outs()-1);
    break;
  case eKeyDownB:
    scrollSelect(-1, net_num_outs()-1);      
    break;
    //// encoder C: scroll target
  case eKeyUpC:
    target++;
    if (target == net_num_ins()) {
      target = -1;
    }
    net_connect(page->selected, target);
    redrawOuts();
    break;
  case eKeyDownC:
    target--;
    if (target == -2) {
      target = net_num_ins() - 1;
    }
    net_connect(page->selected, target);
    redrawOuts();
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

static void keyHandlerGathered(key_t key) {
  keyHandlerOuts(key);
}

//========================================
//======= redraws

//==================================================
//==== redraw ops page
extern void redrawOps(void) {
  U8 y = 0;                       // which line
  S32 n, nCenter;         // which list entry
  //  opStatus_t status = eUserOp;
  // total count of ops, including system-controlled
  const U16 num = net_num_ops();

  // draw the header
  snprintf(lineBuf, SCREEN_W, "         OPERATORS");
  ui_print(0, 0, lineBuf, 6);

  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;

  drawOpLine(n, num, y, 4);
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    drawOpLine(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_2) {
    n++;
    y++;
    drawOpLine(n, num, y, 1);
  }
      
  // draw footer 
  // (new op type)
  snprintf(lineBuf, SCREEN_W, "[ +++ %s",
	   op_registry[userOpTypes[newOpType]].name);
  ui_print(SCREEN_H_2, 0, lineBuf, 1);
  // (function labels)
  // don't allow deletion of system operators
  if (net_op_status(net_num_ops() - 1) == eUserOp) {
    ui_print(SCREEN_H_1, 0, " A_PARAMS   B_ROUTING   C_CREATE  D_DELETE ", 5);
  } else  {
    ui_print(SCREEN_H_1, 0, " A_PARAMS   B_ROUTING   C_CREATE  ", 5);
  }
}

//==================================================
//==== redraw inputs page
extern void redrawIns(void) {
  U8 y = 0;                       // which line
  S32 n, nCenter;         // which list entry
  //  S16 opIdx; // index of operator
  const U16 num = net_num_ins(); // how many ops
    
  // draw the header
  snprintf(lineBuf, SCREEN_W, "       PARAMS ");
  ui_print(0, 0, lineBuf, 6);

  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;
  drawInLine(n, num, y, 4);
  
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    drawInLine(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_1) {
    n++;
    y++;
    drawInLine(n, num, y, 1);
  }    
  // draw footer 
  // (function labels)
  ui_print(SCREEN_H_1, 0, "A_GATHER  B_DISCONNECT  C_STORE  D_PRESET ", 5);
}

//==================================================
//==== redraw outputs page
extern void redrawOuts(void) {
  U8 y = 0;                       // which line
  S32 n, nCenter;         // which list entry
  S16 target;
  const U16 num = net_num_outs(); // how many ops
  
  // draw the header
  snprintf(lineBuf, SCREEN_W, "      ROUTING");
  ui_print(0, 0, lineBuf, 6);

  nCenter = page->selected;
  if (nCenter >= num) {
    nCenter = num;
  }
  n = nCenter;
  // print selection at center
  y = SCREEN_ROW_CENTER;
  drawOutLine(n, num, y, 4);  
  // print lower entries
  while (y > 1) {
    n--;
    y--;
    drawOutLine(n, num, y, 1);
  }
  
  // re-center
  n = nCenter;
  y = SCREEN_ROW_CENTER;
  
  // print higher entries
  while (y < SCREEN_H_2) {
    n++;
    y++;
    drawOutLine(n, num, y, 1);
  }

  // draw footer 
  // (target)
  target = net_get_target(nCenter);
  if(target == -1) {
    ui_print(SCREEN_H_2, 0, "[ NO TARGET ]", 1);
  } else {
    snprintf(lineBuf, SCREEN_W, "  --> %s/%s",
	     net_op_name(net_in_op_idx(target)), net_in_name(target));
    ui_print(SCREEN_H_2, 0, lineBuf, 1);
  }

  // (function labels)
  ui_print(SCREEN_H_1, 0, " A_FOLLOW  B_DISCONNECT C_STORE  D_PRESET ", 5);
}

/// redraw gathered outputs
static void redrawGathered(void) {
}

/////---------------------------
/////  line redraws
static void drawOpLine(S32 n, U16 num, U8 y, U8 hl) {
  if ( (n < num) && (n >= 0) ) { 
    snprintf(lineBuf, SCREEN_W, "   %d__%s",
             (int)n, net_op_name(n));
    ui_print(y, 0, lineBuf, hl + net_op_status(n));
  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, "   .");
    ui_print(y, 0, lineBuf, 0);
  }
  
}

static void drawInLine(S32 n, U16 num, U8 y, U8 hl) {
  S16 opIdx;
  if ( (n < num) && (n >= 0) ) { 
    opIdx = net_in_op_idx(n);
    if (opIdx >=0 ) {
      snprintf(lineBuf, SCREEN_W, "   %d_(%d)%s/%s_%f",
	       (int)n,
	       opIdx, 
	       net_op_name(net_in_op_idx(n)), 
	       net_in_name(n), 
	       net_get_in_value(n));
    } else {

      snprintf(lineBuf, SCREEN_W, "   %d_P%d_%s_%f",
	       (int)n,
	       (int)net_param_idx(n),
	       net_in_name(n), 
	       net_get_in_value(n));
    }
    ui_print(y, 0, lineBuf, hl);

  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, "   .");
    ui_print(y, 0, lineBuf, 0);
  }

}

static void drawOutLine(S32 n, U16 num, U8 y, U8 hl) {
  S16 target; U8 status;
  if ( (n < num) && (n >= 0) ) { 
    target = net_get_target(n);
    status = net_op_status(net_out_op_idx(n));
    if (target >= 0) {
      snprintf(lineBuf, SCREEN_W, "   %d_(%d)%s/%s-->%s/%s",
	       (int)n, net_out_op_idx(n),
	       net_op_name(net_out_op_idx(n)), net_out_name(n), 
	       net_op_name(net_in_op_idx(target)), net_in_name(target)
	       );
    } else {
      snprintf(lineBuf, SCREEN_W, "   %d_(%d)%s/%s",
	       (int)n, net_out_op_idx(n), net_op_name(net_out_op_idx(n)), net_out_name(n));
    }
    ui_print(y, 0, lineBuf, hl + status);
  } else {
    // no selection
    snprintf(lineBuf, SCREEN_W, "   .");
    ui_print(y, 0, lineBuf, 0);
  }
}
