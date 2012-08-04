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
  eKeyEdit,
  eKeyFnDownA,
  eKeyFnUpA,
  eKeyFnDownB,
  eKeyFnUpB,
  eKeyFnDownC,
  eKeyFnUpC,
  eKeyFnDownD,
  eKeyFnUpD,
  eKeyEncUpA,
  eKeyEncDownA,
  eKeyEncUpB,
  eKeyEncDownB,
  eKeyEncUpC,
  eKeyEncDownC,
  eKeyEncUpD,
  eKeyEncDownD,
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
