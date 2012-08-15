/*
  menu.h
  bees
  aleph

  menu logic, public interface
*/

#ifndef _MENU_H_
#define _MENU_H_

#include "types.h"

//-----------------------------------
//----- data types

// top-level state
typedef enum ePage_t {
  ePageOps,
  ePageIns,
  ePageOuts,
  ePagePresets,
  ePageScenes,
  ePageGathered,
  ePagePlay,
  ePageMax
} ePage;

// keys
typedef enum eKey {
  eKeyEdit = 0,
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
extern void menu_handleKey(uiKey_t key);
// parameter feedback
extern void param_feedback(u16 paramIdx);

#endif // header guard
