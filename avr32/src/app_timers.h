#ifndef _ALEPH_AVR32_APP_TIMERS_H_
#define _ALEPH_AVR32_APP_TIMERS_H_

/*
#define TIMER_TAG_SCREEN   0
#define TIMER_TAG_ENCODERS 1
#define TIMER_TAG_SWITCHES 2
*/

// ---- timer tags
enum { 
  eScreenTimerTag = 0,
  eEncTimerTag = 1,
  eSwTimerTag = 2,
};

// create application timers
extern void init_app_timers(void);

#endif


