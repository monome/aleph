/* menu_protected.h
 * bees
 * aleph
 * 
 * menu-related variables for restricted access
 */


#ifndef _MENU_PROTECTED_H_
#define _MENU_PROTECTED_H_

#include "op.h"
#include "net.h"
#include "ui.h"
#include "menu.h"

///////////////////
//////// types

// function pointer for input handling on a given page
typedef void(*keyHandler_t)(uiKey_t key);
// function pointer for redraw on a given page
typedef void(*redraw_t)(void);

// class representing a page in a menu
// includes a title and a key input handler (function pointer)
typedef struct page_struct {
  const char* pageString;
  keyHandler_t keyHandler;
  redraw_t redraw;
  s16 selected;
} page_t;

///////////////////////
///// variables

// pointer to current page
extern page_t* page;
// idx of current page
extern s8 pageIdx;

// array of onode pointers for gathering
extern U32(*gathered)[NET_OUTS_MAX];
// how many gathered
extern u32 numGathered;
// page structures
extern page_t pages[ePageMax];

// const array of user-creatable operator type id's
#define NUM_USER_OP_TYPES 6
#define NUM_USER_OP_TYPES_1 5
extern const opId_t userOpTypes[NUM_USER_OP_TYPES];
// selected new operator type
extern opId_t newOpType;

// list of recently touched parameter idxs
typedef struct _touched {
  U16 idx;
  f32 val;
} touched_t;
extern touched_t touchedParams[SCREEN_H];
 
////////////////////////////
///// functions

// set current page
extern void setPage(ePage n);
// scroll current page
extern void scrollPage(S8 dir);
// scroll selection in current page
extern void scrollSelect(S8 dir, U32 max);

#endif // header guard
