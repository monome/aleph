#ifndef _ALEPH_AVR32_APP_TIMERS_H_
#define _ALEPH_AVR32_APP_TIMERS_H_

// create application timers
extern void init_app_timers(void);

// set the metro timer period
extern void timers_set_metro_ms(u32 ms);

// start monome polling/refresh timers
extern void timers_set_monome(void);

// stop monome polling/refresh timers
extern void timers_unset_monome(void);

#endif
