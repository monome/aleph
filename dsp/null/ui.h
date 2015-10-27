#ifndef _NULLP_UI_H_
#define _NULLP_UI_H_

//#include <ncurses.h>
#include "types.h"

//============== defines
// define key commands
//#define CMD_KEY_QUIT     'q'
#define CMD_KEY_PARAM      'p'
#define CMD_KEY_DEBUG      'd'

typedef enum {
  eUiTop,
  eUiParam,
} uiMode;

extern void init_ui(void);
extern void deinit_ui(void);
extern void handle_key(u16 ch);

#endif
