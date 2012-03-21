/*
  menu.h
  bees
  aleph

  menu logic, public interface
*/

#ifnedf _MENU_H_
#define _MENU_H_

#include "compiler.h"

/////// enums for menu state machine
// top-level state
typedef enum eTopMenuState {
  eMenuStateRoot,   // ROOT menu
  eMenuStatePlay,   // PLAY mode
  eMenuStatePatch,  // PATCH menu
  eMenuStateParam,  // PARAM menu
  eMenuStatePreset, // PRESET menu
  eMenuStateSetup,  // SETUP menu
  eMenuStateMax     // dummy
} topMenuState;

// PARAM submenu substate
typedef enum eParamMenuState {
  eParamMenuStateList,      // editing main list
  eParamMenuStateGathered,  // editing gathered list of outputs
} paramMenuState;

// PARAM selection substate
typedef enum eParamSelectState {
  eParamSelectInput,
  eParamSelectOutput
} paramSelectState;

#endif // header guard
