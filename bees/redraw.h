#ifndef _REDRAW_H_
#define _REDRAW_H_

//#include "compiler.h"
#include "menu.h"
#include "menu_protected.h"

//// page-specific redraws
// ops
extern void redraw_ops(void);
// ins
extern void redraw_ins(void);
// outs
extern void redraw_outs(void);
// gathered outs
extern void redraw_gathered(void);
// presets
extern void redraw_presets(void);
// play
extern void redraw_play(void);
// scene
extern void redraw_scenes(void);
// dsp
extern void redraw_dsp(void);

#endif // header guard
