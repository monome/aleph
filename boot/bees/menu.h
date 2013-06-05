/*
  menu.h
  bees
  aleph

  menu logic, public interface
*/

#ifndef _MENU_H_
#define _MENU_H_

#include "fix.h"
#include "types.h"

//-----------------------------------
//----- data types
#define NUM_PAGES 2

// top-level state
typedef enum ePage_t {
  ePageDsp,
  ePageFw
} ePage;

// keys
typedef enum eKey {
  eKeyMode = 0,
  eKeyFnDownA = 1,
  eKeyFnUpA = 2,
  eKeyFnDownB = 3,
  eKeyFnUpB = 4,
  eKeyFnDownC = 5,
  eKeyFnUpC = 6,
  eKeyFnDownD = 7,
  eKeyFnUpD = 8,
  eKeyEncUpA = 9,
  eKeyEncDownA = 10 ,
  eKeyEncUpB = 11 ,
  eKeyEncDownB = 12 ,
  eKeyEncUpC = 13 ,
  eKeyEncDownC = 14 ,
  eKeyEncUpD = 15 ,
  eKeyEncDownD = 16,
  eKeyDummy = 17
} uiKey_t;

//----- public functions
// init
extern void menu_init(void);
// de-init
extern void menu_deinit(void);
// handle key
extern void menu_handleKey(uiKey_t key, s16 value);
// parameter feedback
extern void param_feedback(u16 paramIdx, fix16 val);

// refresh the current page
extern void menu_refresh(void);

			      
#endif // header guard
