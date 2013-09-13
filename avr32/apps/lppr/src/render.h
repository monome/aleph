#ifndef _ALEPH_APP_LPPR_RENDER_H_
#define _ALEPH_APP_LPPR_RENDER_H_


// init
extern void render_init(void);
// update
extern void render_update(void);
// force refresh
extern void render_force_refresh(void);

// render text to statusbar
void render_status(const char* str);

// fill with initial graphics (id strings)
void render_startup(void);

// draw switch-on
 extern void render_sw_on(u8 sw, u8 on);
// draw delay time
extern void render_delay_time(u8 id, u32 ms, u32 samps);
// draw amplitude (assume amp is precalculated but db is not)
extern void render_amp(u32 input, s32 amp);
// draw frequency
extern void render_freq(u32 input);

// clear the main region when a new knob is touched
//extern void render_new_param_touch(void);

// render parameter values when they change
//extern void render_delaytime();
//extern void render_fb(u32 in);
extern void render_freq(u32 in);
extern void render_res(u32 in);
extern void render_mix(u32 in);
extern void render_panIn(u32 in);
extern void render_panOut(u32 in);

// render labels when a new parameter is touched
extern void render_touched_delaytime(u8 id);
extern void render_touched_fb(u8 id);
extern void render_touched_mix(u8 id);
extern void render_touched_freq(u8 id);
extern void render_touched_res(u8 id);
//extern void render_touched_panIn(u8 id);
//extern void render_touched_panOut(u8 id);

#endif // h guard
