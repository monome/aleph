//render.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_RENDER_H_
#define _ALEPH_APP_PRGM_RENDER_H_

#define STRBUF_LEN 16
#define NUMSTRBUF_LEN 10

//note: 12 (1), 411 (152)| just: 12 (1), 256 (95)
//RENDER STARTUP
#define RATIO_VAL_INIT 12
#define FREQ_VAL_INIT 411

//TRACKER STEPS
#define INIT_F 152 * 0x00010000
#define INIT_T 1 * 0x00010000

char numstrbuf[NUMSTRBUF_LEN];

extern void render_init(void);
extern void render_boot(const char *str);
extern void render_startup(void);
extern void render_update(void);

//PRGM
extern void render_prgm(void);
extern void render_wave(void);
extern void render_phase(void);
extern void render_blend(void);

//TRACKER
extern void render_tracker(void);
extern void render_freq(void);
extern void render_transpose(void);
extern void render_counters(void);

#endif

