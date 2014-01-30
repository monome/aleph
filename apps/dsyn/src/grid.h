#ifndef _APP_DSYN_GRID_H_
#define _APP_DSYN_GRID_H_

#include "types.h"

// scroll the position of the grid editor relative to the seauence
extern void grid_inc_scroll(s8 inc);
// handle a grid/key event
extern void grid_handle_key_event(s32 data);
// set size
extern void grid_set_size(u8 w, u8 h);
// display sequence
extern void grid_show_seq(void);
// display position
extern void grid_show_pos(void);
// show loop start point
extern void grid_show_start(void);
// show loop lenth / end poin
extern void grid_show_len(void);
// show page
extern void grid_show_page(void);
// toggle edit mode
extern void grid_toggle_edit_mode(void);


#endif
