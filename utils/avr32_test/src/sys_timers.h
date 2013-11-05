#ifndef _ALEPH_AVR32_SYS_TIMERS_H_
#define _ALEPH_AVR32_SYS_TIMERS_H_

// ---- timer tags
///////// FIXME: 
//-------- dumb, but just keep adding tags here for now
enum { 
  eEncTimerTag,
  eAdcTimerTag,
  eMonomePollTimerTag,
  eMonomeRefreshTimerTag,
  eParamChangeTimerTag,
  eNumSysTimers
};

// create application timers
extern void init_sys_timers(void);

#endif
