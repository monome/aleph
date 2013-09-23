#ifndef _ALEPH_APP_FLRY_RENDER_H_
#define _ALEPH_APP_FLRY_RENDER_H_


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

// draw text in a region
// extern void 

// draw a buffer in a quadrant
//extern void render_quad(u8 quad, u8* data);

// draw switch-on
 extern void render_sw_on(u8 sw, u8 on);
// draw dac value
 extern void render_dac(u8 ch, s32 val);

extern void render_param(u8 vid, s32 idx, s32 val);
#endif // h guard
