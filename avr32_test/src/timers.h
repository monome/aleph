/* timers.c
 * aleph
 *
 * ultra-simple software timer API
 */


#ifndef _TIMERS_H_
#define _TIMERS_H_

#include <compiler.h>

// maximum number of timers
#define MAX_TIMERS (12)
// timer interrupt priority level
//#define TIMER_INT_LEVEL 1

//----- types

// callback function prototype
typedef void (*timerCallback)( int tag );

// timer descriptor struct
typedef struct _swTimer {
  int        fperiodic;   	// !0 for periodic timer, else one-shot 
  int        tag;        	// unique ID value 
  int        timeout;           // timer expiration period in ticks
  int        timeoutReload;     // reload value for periodic timer 
  timerCallback callback;   	// callback function prototype 
} swTimer_t;

//---- functions
// create a timer with a callback
bool set_timer( swTimer_t * timer, int tag, int ticks,
		timerCallback callback, bool fPeriodic );
// kill a timer
bool kill_timer( int tag );
// initialize timer engine
void init_timers( void );
// this should be called every tick period
void process_timers( void );

#endif // header guard
