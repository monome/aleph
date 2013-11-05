#ifndef _ALEPH_AVR32_APP_TIMERS_H_
#define _ALEPH_AVR32_APP_TIMERS_H_


// ---- timer tags
enum { 
  eScreenTimerTag = 0,
  eEncTimerTag = 1,
  eSwTimerTag = 2,
  eAdcTimerTag = 3,
  eHeartbeatTimerTag = 4,
};

// create application timers
extern void init_app_timers(void);

#endif
