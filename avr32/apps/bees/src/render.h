#ifndef _RENDER_H_
#define _RENDER_H_

//#include "compiler.h"
#include "menu.h"
#include "menu_protected.h"

//// page-specific renders
// ops
extern void render_ops(void);
// ins
extern void render_ins(void);
// outs
extern void render_outs(void);
// gathered outs
extern void render_gathered(void);
// presets
extern void render_presets(void);
// play
extern void render_play(void);
// scene
extern void render_scenes(void);
// dsp
extern void render_dsp(void);

#endif // header guard
