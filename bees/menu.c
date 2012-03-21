/*
  menu.c
  bees
  aleph
*/

#include "menu.h" 

//------- static variables
static topMenuState topState;
static paramMenuState paramState;

// which line is currently selected
// (usually an index in a list of patches/params/inputs/whatever)
static S16 selected = 0;

//------- static functions

static void scroll(S8 dir) {
  switch(topState) {
  case eMenuStateRoot:
    // nothing
    break;
  case eMenuStatePlay:
    // nothing
    break;
  case eMenuStatePatch:
    selected += dir;
    if (selected < 0) { selected = 0; }
    if (selected > (patch_num_patches -1)) { selected = patch_num_patches - 1; }
    break;
  case eMenuStateParam:
    selected += dir;
    if (selected < 0) { selected = 0; }
    if (selected > (param_num_params -1)) { selected = param_num_params - 1; }
    break;
  case eMenuStatePreset:
    selected += dir;
    if (selected < 0) { selected = 0; }
    if (selected > (preset_num_presets -1)) { selected = preset_num_presets - 1; }
    break;
    break;
  case eMenuStateSetup:
    break;
  default:
    break;
  }
}

static void gatherInputs(void) {
}

static void
