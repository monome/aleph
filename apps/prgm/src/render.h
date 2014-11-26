//render.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_RENDER_H_
#define _ALEPH_APP_PRGM_RENDER_H_

#define STRBUF_LEN 16
#define NUMSTRBUF_LEN 10

char numstrbuf[NUMSTRBUF_LEN];

extern void render_init(void);
extern void render_boot(const char *str);
extern void render_startup(void);
extern void render_update(void);

//page LEVEL
extern void render_row1(void);
extern void render_row2(void);
extern void render_row3(void);
extern void render_countlev(void);

//page ENV
extern void render_trig(void);
extern void render_time(void);
extern void render_mode(u8 chan, s32 name);
extern void render_modes(u8 i);
extern void render_countenv(void);

#endif
