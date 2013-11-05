#ifndef _KEY_HANDLER_H_
#define _KEY_HANDLER_H_

//#include "compiler.h"
#include "menu.h"
#include "menu_protected.h"

//// page-specific key handlers
// dsp
extern void key_handler_dsp(uiKey_t key, s16 val);
// firmware
extern void key_handler_fw(uiKey_t key, s16 val);

#endif // _KEY_HANDLER_H_
