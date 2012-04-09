/*
  menu.h
  bees
  aleph

  menu logic, public interface
*/

#ifndef _MENU_H_
#define _MENU_H_

#include "../common/types.h"


//-----------------------------------
//----- data types

// top-level state
typedef enum ePage_t {
	ePageOps,
	ePageIns,
	ePageOuts,
	ePageMax     // dummy
} ePage;

// keys
typedef enum eKey {
  eKeyLeft,
  eKeyRight,
  eKeyFunctionA,
  eKeyFunctionB,
  eKeyFunctionC,
  eKeyFunctionD,
  eKeyUpA,
  eKeyDownA,
  eKeyUpB,
  eKeyDownB,
  eKeyUpC,
  eKeyDownC,
  eKeyUpD,
  eKeyDownD,
  eKeyMax // dummy
} key_t;

// function pointer for input handling on a given page
typedef void(*keyHandler_t)(key_t key);
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

//----- public functions
// init
extern void menu_init(void);
// de-init
extern void menu_deinit(void);
// handle key
extern void menu_handleKey(key_t key);


#endif // header guard
