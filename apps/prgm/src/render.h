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
extern void render_tracker(void);
extern void render_free(void);
extern void render_transposed(void);
extern void render_counters(void);

//page ENV
extern void render_time(void);
extern void render_curve(void);
extern void render_dest(void);
extern void render_trig(void);

#endif

