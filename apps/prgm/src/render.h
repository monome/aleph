//prgm
//aleph-avr32

#ifndef _ALEPH_APP_PRGM_RENDER_H_
#define _ALEPH_APP_PRGM_RENDER_H_

#define STRBUF_LEN 16
#define NUMSTRBUF_LEN 10

char numstrbuf[NUMSTRBUF_LEN];

extern void render_init(void);

extern void render_startup(void);

extern void render_update(void);

extern void render_boot(const char *str);

extern void render_prgm(void);
extern void render_freq(void);

extern void render_tracker(void);

#endif // h guard
