/* play.h 
   bees
   aleph

   interface for play mode UI
 */

#ifndef _ALEPH_BEES_PLAY_H_
#define _ALEPH_BEES_PLAY_H_

#include "op.h"
#include "param_common.h"


// number of entries in the scroll
#define  PLAY_SCROLL_NUM 7 // CHAR_ROWS_1
//#define  PLAY_SCROLL_NUM__2 7 // CHAR_ROWS_1
// length of each entry
#define PLAY_STR_LEN 28
// size of buffer (product)
#define PLAY_SCROLL_BUF_LEN 196

// init
extern void play_init(void); 
// process input node
extern void play_input(u16 idx);
// enable rendering (play modal page was selected) 
extern void play_enable_render(void);

// process preset change
extern void play_preset(u16 idx);

#endif // h guard
