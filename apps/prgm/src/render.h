//render.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_RENDER_H_
#define _ALEPH_APP_PRGM_RENDER_H_

#define STRBUF_LEN 16
#define NUMSTRBUF_LEN 10

//init values, cv
#define INIT_F 13421773 //0x7ffffff
#define INIT_T 1 * 0x00010000

#define FREE_VAL_INIT 264 //note: 411 (152)| just: 264 (152)
#define TRANSPOSED_VAL_INIT 12
//init values, tracker
//#define INIT_F 152 * 0x00010000
//#define INIT_T 1 * 0x00010000

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


extern void render_attack(void);
extern void render_decay(void);
extern void render_loop(void);

#endif

