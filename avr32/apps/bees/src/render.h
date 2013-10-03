#ifndef _RENDER_H_
#define _RENDER_H_

// avr32
#include "region.h"
// bees
#include "menu.h"
#include "menu_protected.h"

//// page-specific renders
// ops
/* extern void render_ops(void); */
/* // ins */
/* extern void render_ins(void); */
/* // outs */
/* extern void render_outs(void); */
/* // gathered outs */
/* extern void render_gathered(void); */
/* // presets */
/* extern void render_presets(void); */
/* // play */
/* extern void render_play(void); */
/* // scene */
/* extern void render_scenes(void); */
/* // dsp */
/* extern void render_dsp(void); */

// draw dirty regions to the screen
extern void render_update(void);

// set current header region
void render_set_head_region(region* reg);

// set current footer region
void render_set_foot_region(region* reg);

// set current scroll region
void render_set_scroll_region(region* reg) ;


#endif // header guard
