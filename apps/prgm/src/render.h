//render.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_RENDER_H_
#define _ALEPH_APP_PRGM_RENDER_H_
//--- std headers
#include <string.h>

//debug
#include "print_funcs.h"

//avr32-lib
#include "fix.h"
#include "app.h"
#include "font.h"
#include "memory.h"
#include "region.h"
#include "screen.h"
#include "control.h"

//prgm
#include "pages.h"
#include "page_mix.h"
#include "page_seq.h"
#include "page_gen.h"

#include "tracker.h"
#include "generator.h"

#include "util.h"

#define STRBUF_LEN 16
// center line
#define SCROLL_CENTER_LINE 	3
// lines above center
#define SCROLL_LINES_BELOW 	3
#define SCROLL_LINES_ABOVE 	4
#define SCROLL_LINES_ABOVE_1 	(SCROLL_LINES_ABOVE - 1)
#define SCROLL_BYTES_PER_LINE (FONT_CHARH * 128)
// byte offset for top of center row
#define SCROLL_CENTER_OFFSET (SCROLL_CENTER_LINE * SCROLL_BYTES_PER_LINE)
// in pixels, y-offset for top of center row
#define SCROLL_CENTER_Y_OFFSET (SCROLL_CENTER_LINE * FONT_CHARH)
/// offset for last row
#define SCROLL_LAST_LINE_OFFSET (SCROLL_BYTES_PER_LINE * 7)
// underline at bottom of line region
#define LINE_UNDERLINE_LEN 127
#define LINE_UNDERLINE_OFFSET (SCROLL_BYTES_PER_LINE - LINE_UNDERLINE_LEN - 1)

//x-offset in pixels for value field on each line
#define NUMSTRBUF_LEN 10
#define LINEBUF_LEN 32
#define NUMBUF_LEN 4
#define LINE_VAL_POS_PARAM1 32
#define LINE_VAL_POS_PARAM2 64
#define SCROLL_OFFSET 3

char numstrbuf[NUMSTRBUF_LEN];

extern void render_init(void);
extern void render_boot(const char *str);
extern void render_startup(void);
extern void render_update(void);

//page TRK
extern void render_track(u8 track);

//page MIX
extern void render_mixtrack(u8 region_a, u8 region_b, u8 region_c, u8 track, u8 mode);
//extern void render_mixcounters(void);

//page SEQ
extern void scroll_sequence(u8 track);
extern void render_seqtrack(u8 track);

//page GEN
extern void render_gen_param(u8 genpos);

extern void render_gstrackname(u8 track);
extern void render_gsdest(u8 destination);
extern void render_gscounters(void);

extern void region_gstrig(u8 track);
extern void region_gstime(u8 track);

extern void render_sample(u8 region, s32 sample);

extern void render_param(void);
extern void render_track0(u8 mode);
extern void render_track1(u8 mode);
extern void render_track2(u8 mode);
extern void render_track3(u8 mode);
extern void render_inputA(u8 region, s32 name);
extern void render_inputB(u8 region, s32 name);
extern void render_sample_param(u8 region, s32 parameter);
extern void render_sample_env(u8 region, s32 env);

extern void render_aux_param(u8 track);
extern void render_steplength_param(u8 pos);
extern void render_counters_param(void);

extern void render_env(void);
extern void render_mode(u8 track, s32 name);
extern void render_modes(u8 i);

extern void render_aux_env(u8 track);
extern void render_steplength_env(u8 pos);
extern void render_counters_env(void);




#endif

