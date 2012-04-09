/*
  menu.c
  bees
  aleph
*/

#include "ui.h"
#include "menu.h"

//-----------------------------------
//----- static function declarations

// set current page
static void setPage(ePage n);
// refresh whatever for current page
static void menuRedraw(void);

// print the top menu selections
static void printTopMenu(void);
// print operator list given current selection
static void printOpList(void);
// print parameter list given current selection
static void printParamList(void);

// find and print inputs to selected parameter
static void gatherInputs(void);

//// page-specific key handlers
static void keyHandlerRoot(key_t key);
static void keyHandlerPlay(key_t key);
static void keyHandlerPatch(key_t key);
static void keyHandlerParam(key_t key);
static void keyHandlerPreset(key_t key);
static void keyHandlerSetup(key_t key);

//// page-specific redraws
static void redrawRoot(void);
static void redrawPlay(void);
static void redrawPatch(void);
static void redrawParam(void);
static void redrawPreset(void);
static void redrawSetup(void);
static void redrawSetup(void);

//-----------------------------------
//------- static variables

// page titles - syncronize with ePage enum
static page_t pages[ePageMax] = {
{ "ROOT",   (keyHandler_t)&keyHandlerRoot,    (redraw_t)&redrawRoot, 0 },
{ "PLAY",   (keyHandler_t)&keyHandlerPlay,    (redraw_t)&redrawPlay , 0 },
{ "PATCH",  (keyHandler_t)&keyHandlerPatch,   (redraw_t)&redrawPatch , 0 },
{ "PARAM",  (keyHandler_t)&keyHandlerParam,   (redraw_t)&redrawParam , 0 },
{ "PRESET", (keyHandler_t)&keyHandlerPreset,  (redraw_t)&redrawPreset , 0 },
{ "SETUP",  (keyHandler_t)&keyHandlerSetup,   (redraw_t)&redrawSetup , 0 }
};

// which page
static page_t page;
// which line is currently selected
// (usually an index in a list of patches/params/inputs/whatever)
// static S16 selected = 0;



//-----------------------------------
//----- external function definitions

// init
extern void menu_init(void) {
  setPage(ePageRoot);
}

// de-init
extern void menu_deinit(void) {
}

// top level key handler
void handleKey(key_t key) {
  page.keyHandler(key);
}

//-----------------------------------
//----- static function definitions

// set current page
static void setPage(ePage n) {
  page = pages[n];
  page.redraw();
}

// refresh current page
void menuRedraw(void) {
  page.redraw();
}

// find and print connected output nodes
static void gatherInputs(void) {
}

//========================================
//====== key handlers
// ROOT
void keyHandlerRoot (key_t key) {
  switch(key) {
    case eKeyLeft:
      // nothing
      break;
    case eKeyRight:
      // go to the currently selected menu
      if ( (page.selected > 0)
          && (page.selected != ePageRoot)
          && (page.selected < ePageMax)
          ) {
        setPage(page.selected);
      }
      break;
    case eKeyFunctionA:
      break;
    case eKeyFunctionB:
      break;
    case eKeyFunctionC:
      break;
    case eKeyFunctionD:
      break;
      //// encoder A: scroll slow
    case eKeyUpA:
      page.selected += 1;
      if (page.selected >= ePageMax) {
        page.selected = ePageMax - 1;
        redrawRoot();
      }
      break;
    case eKeyDownA:
      page.selected -= 1;
      if (page.selected < 0) {
        page.selected = 0;
        redrawRoot();
      }
      break;
      //// encoder B: scroll fast
    case eKeyUpB:
      // nothing
      break;
    case eKeyDownB:
      // nothing
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

// PLAY
void keyHandlerPlay (key_t key) {
  switch(key) {
    case eKeyLeft:
      break;
    case eKeyRight:
      break;
    case eKeyFunctionA:
      break;
    case eKeyFunctionB:
      break;
    case eKeyFunctionC:
      break;
    case eKeyFunctionD:
      break;
    case eKeyUpA:
      break;
    case eKeyDownA:
      break;
    case eKeyUpB:
      break;
    case eKeyDownB:
      break;
    case eKeyUpC:
      break;
    case eKeyDownC:
      break;
    case eKeyUpD:
      break;
    case eKeyDownD:
      break;
    case eKeyMax: // dummy
      break;
      //default:
  }  
}


// PATCH
void keyHandlerPatch (key_t key) {
  switch(key) {
    case eKeyLeft:
      break;
    case eKeyRight:
      break;
    case eKeyFunctionA:
      break;
    case eKeyFunctionB:
      break;
    case eKeyFunctionC:
      break;
    case eKeyFunctionD:
      break;
    case eKeyUpA:
      break;
    case eKeyDownA:
      break;
    case eKeyUpB:
      break;
    case eKeyDownB:
      break;
    case eKeyUpC:
      break;
    case eKeyDownC:
      break;
    case eKeyUpD:
      break;
    case eKeyDownD:
      break;
    case eKeyMax: // dummy
      break;
      //default:
  }  
}

// PARAM
void keyHandlerParam (key_t key) {
  switch(key) {
    case eKeyLeft:
      break;
    case eKeyRight:
      break;
    case eKeyFunctionA:
      break;
    case eKeyFunctionB:
      break;
    case eKeyFunctionC:
      break;
    case eKeyFunctionD:
      break;
    case eKeyUpA:
      break;
    case eKeyDownA:
      break;
    case eKeyUpB:
      break;
    case eKeyDownB:
      break;
    case eKeyUpC:
      break;
    case eKeyDownC:
      break;
    case eKeyUpD:
      break;
    case eKeyDownD:
      break;
    case eKeyMax: // dummy
      break;
      //default:
  }  
}


// PRESET
void keyHandlerPreset (key_t key) {
  switch(key) {
    case eKeyLeft:
      break;
    case eKeyRight:
      break;
    case eKeyFunctionA:
      break;
    case eKeyFunctionB:
      break;
    case eKeyFunctionC:
      break;
    case eKeyFunctionD:
      break;
    case eKeyUpA:
      break;
    case eKeyDownA:
      break;
    case eKeyUpB:
      break;
    case eKeyDownB:
      break;
    case eKeyUpC:
      break;
    case eKeyDownC:
      break;
    case eKeyUpD:
      break;
    case eKeyDownD:
      break;
    case eKeyMax: // dummy
      break;
      //default:
  }  
}

// SETUP
void keyHandlerSetup (key_t key) {
  switch(key) {
    case eKeyLeft:
      break;
    case eKeyRight:
      break;
    case eKeyFunctionA:
      break;
    case eKeyFunctionB:
      break;
    case eKeyFunctionC:
      break;
    case eKeyFunctionD:
      break;
    case eKeyUpA:
      break;
    case eKeyDownA:
      break;
    case eKeyUpB:
      break;
    case eKeyDownB:
      break;
    case eKeyUpC:
      break;
    case eKeyDownC:
      break;
    case eKeyUpD:
      break;
    case eKeyDownD:
      break;
    case eKeyMax: // dummy
      break;
      //default:
  }  
}

//========================================
//======= redraws

extern void redrawRoot(void) {
}

extern void redrawPlay(void) {
}

extern void redrawPatch(void) {
}

extern void redrawParam(void) {
}

extern void redrawPreset(void) {
}

extern void redrawSetup(void) {
}

