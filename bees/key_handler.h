#ifndef _KEY_HANDLER_H_
#define _KEY_HANDLER_H_

//#include "compiler.h"
#include "menu.h"
#include "menu_protected.h"

//// page-specific key handlers
// ops
extern void key_handler_ops(key_t key);
// ins
extern void key_handler_ins(key_t key);
// outs
extern void key_handler_outs(key_t key);
// gathered outs
extern void key_handler_gathered(key_t key);
// presets
extern void key_handler_presets(key_t key);
// play
extern void key_handler_play(key_t key);

#endif // _KEY_HANDLER_H_
