#ifndef _ALEPH_AVR32_APP_TIMERS_H_
#define _ALEPH_AVR32_APP_TIMERS_H_

// create application timers
extern void init_app_timers(void);

// start monome polling/refresh timers
extern void timers_set_monome(void);

// stop monome polling/refresh timers
extern void timers_unset_monome(void);

#endif

/* #ifndef _ALEPH_AVR32_APP_TIMERS_H_ */
/* #define _ALEPH_AVR32_APP_TIMERS_H_ */

/* #include "sys_timers.h" */

/* // ---- timer tags */
/* enum {  */
/*   eScreenTimerTag = eNumSysTimers, */
/*   eTestTag, */
/*   //  eAdcTimerTag, */
/*   //  eMonomePollTimerTag, */
/*   //  eMonomeRefreshTimerTag, */
/*   //  eMidiPollTimerTag, */
/*   //  eMidiRefreshTimerTag, */
/* }; */

/* // create application timers */
/* extern void init_app_timers(void); */

/* extern void kill_test(void); */

/* #endif */
