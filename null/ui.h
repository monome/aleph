#ifndef _NULLP_UI_H_
#define _NULLP_UI_H_

//#include <ncurses.h>
#include "types.h"

//============== defines
// define key commands
#define CMD_KEY_QUIT     'q'
#define CMD_KEY_LOAD     'l'
#define CMD_KEY_UP       KEY_UP
#define CMD_KEY_DOWN     KEY_DOWN
#define CMD_KEY_LEFT     KEY_LEFT
#define CMD_KEY_RIGHT    KEY_RIGHT
#define CMD_KEY_PARAM      'p'
#define CMD_KEY_VALUE      'v'
#define CMD_KEY_SWITCH1    '-'
#define CMD_KEY_SWITCH2    '='
#define CMD_KEY_INC_COARSE  '>'
#define CMD_KEY_INC_FINE    '.'
#define CMD_KEY_DEC_COARSE  '<'
#define CMD_KEY_DEC_FINE    ','
#define CMD_KEY_STORE_SCENE 's'
#define CMD_KEY_RECALL_SCENE 'r'
#define CMD_KEY_SCENE_FLAG 'x'
#define CMD_KEY_RAMP_TIME 't'
#define CMD_KEY_HELP        '?'

typedef enum {
  eUiTop,
  eUiParam,
} uiMode;

extern void init_ui(void);
extern void deinit_ui(void);
extern void handle_key(u16 ch);

#endif
