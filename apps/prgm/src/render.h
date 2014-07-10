//prgm
//aleph-avr32

#ifndef _ALEPH_APP_PRGM_RENDER_H_
#define _ALEPH_APP_PRGM_RENDER_H_

#define STRBUF_LEN 16
#define NUMSTRBUF_LEN 10

char numstrbuf[NUMSTRBUF_LEN];

extern void render_init(void);
extern void render_boot(const char *str);
extern void render_startup(void);

extern void render_update(void);

//PRGM
extern void render_prgm(void);
extern void render_phase(void);
extern void render_blend(void);

//TRACKER
extern void render_tracker(void);
extern void render_freq(void);

#endif // h guard
