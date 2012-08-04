#ifndef _KEY_HANDLER_H_
#define _KEY_HANDLER_H_

//#include "compiler.h"
#include "menu.h"
#include "menu_protected.h"

//// page-specific key handlers
// ops
extern void key_handler_ops(uiKey_t key);
// ins
extern void key_handler_ins(uiKey_t key);
// outs
extern void key_handler_outs(uiKey_t key);
// gathered outs
extern void key_handler_gathered(uiKey_t key);
// presets
extern void key_handler_presets(uiKey_t key);
// scenes
extern void key_handler_scenes(uiKey_t key);
// play
extern void key_handler_play(uiKey_t key);

#endif // _KEY_HANDLER_H_
