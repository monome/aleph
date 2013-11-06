/* menu_protected.h
 * bees
 * aleph
 * 
 * menu-related variables for restricted access
 */


#ifndef _MENU_PROTECTED_H_
#define _MENU_PROTECTED_H_

#include "menu.h"
#include "screen.h"

///////////////////
//////// types

// function pointer for input handling on a given page
typedef void(*keyHandler_t)(uiKey_t key);
// function pointer for redraw on a given page
typedef void(*redraw_t)(void);

// messy page mode enum
typedef enum  { 
   eModeNone,
   eModeClear,
   eModeCopy,
   eModeStore,
   eModeRecall,
} ePageMode;

// class representing a page in a menu
// includes a title and a key input handler (function pointer)
typedef struct page_struct {
  const char* pageString;
  keyHandler_t keyHandler;
  redraw_t redraw;
  s16 selected;
  ePageMode mode;
  s8 cursor;
  // knob sensitivities for this page
  s8 encSens[4];
} page_t;

///////////////////////
///// variables

// pointer to current page
extern page_t* curPage;
// idx of current page
extern s8 pageIdx;

// array of onode pointers for gathering
// page structures
extern page_t pages[NUM_PAGES];

 
////////////////////////////
///// functions

// set current page
extern void set_page(ePage n);
// scroll current page
extern void scroll_page(s8 dir);
// scroll selection in current page
extern void scroll_select(s8 dir, s32 max);
extern void scroll_select_clip(s8 dir, s32 max);
extern void scroll_select_wrap(s8 dir, s32 max);


#endif // header guard
