#ifndef _ALEPH_APP_MIX_RENDER_H_
#define _ALEPH_APP_MIX_RENDER_H_


// init
extern void render_init(void);

// startup state
extern void render_startup(void);

// render a channel
extern void render_chan(u8 ch);

// update
extern void render_update(void);

#endif // h guard
