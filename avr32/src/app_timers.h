#ifndef _ALEPH_AVR32_APP_TIMERS_H_
#define _ALEPH_AVR32_APP_TIMERS_H_

// ---- timer tags
enum { 
  eScreenTimerTag,
  eEncTimerTag,
  eSwTimerTag,
  eAdcTimerTag,
  eHeartbeatTimerTag,
  eMonomePollTimerTag,
  eMonomeRefreshTimerTag,
  eMidiPollTimerTag,
  eMidiRefreshTimerTag,
  eParamChangeTimerTag,
};

// create application timers
extern void init_app_timers(void);

#endif
