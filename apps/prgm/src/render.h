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

//page PARAM
extern void render_param(void);
extern void render_track0(u8 mode);
extern void render_track1(u8 mode);
extern void render_track2(u8 mode);
extern void render_track3(u8 mode);
extern void render_inputA(u8 track, s32 name);
extern void render_inputB(u8 track, s32 name);
extern void render_sample(u8 track, s32 sample);

extern void render_aux_param(u8 track);
extern void render_steplength_param(u8 pos);
extern void render_measure_param(u8 pos);
extern void render_bufferposition_param(u8 pos);
extern void render_tempo_param(void);
extern void render_counters_param(void);

//page ENV
extern void render_trig(void);
extern void render_time(void);
extern void render_mode(u8 track, s32 name);
extern void render_modes(u8 i);

extern void render_aux_env(u8 track);
extern void render_steplength_env(u8 pos);
extern void render_measure_env(u8 pos);
extern void render_bufferposition_env(u8 pos);
extern void render_tempo_env(void);
extern void render_counters_env(void);




#endif

