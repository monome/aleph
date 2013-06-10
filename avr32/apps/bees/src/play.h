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
#define  PLAY_SCROLL_NUM 14 // CHAR_ROWS_1 * 2
#define  PLAY_SCROLL_NUM__2 7 // CHAR_ROWS_1

// size of text buffer at each entry
#define PLAY_SCROLL_BUF_LEN 25


// list of recently touched parameter idxs
/* typedef struct _touched { */
/*   u16 idx; */
/*   io_t val; */
/*   char name[PARAM_LABEL_LEN]; */
/* } touched_t; */

// init
void play_init(void); 
// add parameter data to scroll
void play_param(u16 paramIdx);

// get text buffer for given entry
const char * play_get_entry(u16 idx);

#endif // h guard
