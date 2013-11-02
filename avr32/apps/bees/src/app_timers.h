#ifndef _ALEPH_AVR32_APP_TIMERS_H_
#define _ALEPH_AVR32_APP_TIMERS_H_

#include "sys_timers.h"

// ---- timer tags
enum { 
  eScreenTimerTag = eNumSysTimers,
  eAdcPollTimerTag,
  eMonomePollTimerTag,
  eMonomeRefreshTimerTag,
  eMidiPollTimerTag,
  eMidiRefreshTimerTag,
};

// create application timers
extern void init_app_timers(void);

// start monome polling/refresh timers
extern void timers_set_monome(void);

// stop monome polling/refresh timers
extern void timers_unset_monome(void);

// start midi polling/refresh timers
extern void timers_set_midi(void);

// stop midi polling/refresh timers
extern void timers_unset_midi(void);

// start adc polling timer
extern void timers_set_adc(void);

// stop adc polling timer
extern void timers_unset_adc(void);

#endif
