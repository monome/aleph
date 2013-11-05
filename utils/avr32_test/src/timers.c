/* timers.c
 * aleph
 *
 * ultra-simple timer multiplexing API.
 * set_timer() and kill_timer() disable interrupts and so are safe to call from ISRs.
 */

#include "aleph_board.h"
#include "conf_tc_irq.h"
#include "timers.h"

//-----------------------------------------------
//---- static variables

// pointers to timers
static swTimer_t*  timers[ MAX_TIMERS ];

//-----------------------------------------------
//---- static functions
static swTimer_t* find_timer( timerCallback callback, int tag );
static bool add_timer( swTimer_t* newTimer );

// find a timer given a tag and optional callback
static swTimer_t* find_timer( timerCallback callback, int tag ) {
  int k;
  swTimer_t* t;

  //  bool fReenableInterrupts = Is_interrupt_level_enabled( TIMER_INT_LEVEL );
  //  Disable_interrupt_level( TIMER_INT_LEVEL );
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);

  for ( k = 0; k < MAX_TIMERS; k++ ) {
    t = timers[k];
    if ( t == NULL ) {
      continue;
    }

    if ( t->tag == tag ) {
      if ( callback != NULL ) {
	if ( t->callback == callback ) {
	  // tag and callback both specified and matched
	  //	  if (fReenableInterrupts)
	  //	    Enable_interrupt_level( TIMER_INT_LEVEL );
	  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
	  return t;
	} else {
	  // tag and callback specified, but only tag matched
	  continue;
	}
      }

      // only tag specified, matched
      //      if (fReenableInterrupts) {
      //	Enable_interrupt_level( TIMER_INT_LEVEL ); 
      cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
      return t;
	//      }
    }
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  /* if (fReenableInterrupts) { */
  /*   Enable_interrupt_level( TIMER_INT_LEVEL ); */
  /* } */

  return NULL;
}

// Add timer to pointer array. Finds first empty slot.
static bool add_timer( swTimer_t* newTimer) {
  int k;
  //  bool fReenableInterrupts = Is_interrupt_level_enabled( TIMER_INT_LEVEL );
  //  Disable_interrupt_level( TIMER_INT_LEVEL );
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  
  // find empty slot
  for ( k = 0; k < MAX_TIMERS; k++ ) {
    if ( timers[k] ==  NULL ) {
      timers[k] = newTimer;
      cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
      //      Enable_interrupt_level( TIMER_INT_LEVEL );
      return true;
    }
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  //  if (fReenableInterrupts) {
  //    Enable_interrupt_level( TIMER_INT_LEVEL );
  //  }

  return false;
}

//-----------------------------------------------
//---- external functions

// initialize the timer system.
void init_timers( void ) {
  int k;

  for ( k = 0; k < MAX_TIMERS; k++ ) {
    timers[ k ] = 0;
  }
}

// Add a callback timer to the list.
bool set_timer(  swTimer_t* t, int tag, int ticks, timerCallback callback,
		 bool fPeriodic ) {
  if ( callback == NULL ) {
    return false;
  }
  // dont add if callback appears in the list
  if ( find_timer( callback, tag ) != NULL ) {
    return false;
  }

  t->callback = callback;
  t->tag = tag;
  t->timeout = ticks;
  t->fperiodic = fPeriodic;
  t->timeoutReload = ticks;

  return add_timer( t );
}

// kill a timer given its tag value.
bool kill_timer( int tag ) {
  int k;
  
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);

  for ( k = 0; k < MAX_TIMERS; k++ ) {
    if ( timers[k]->tag == tag ) {
      timers[k] = NULL;
      cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
      return true;
    }
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);

  return false;
}


// process the list of timers.
// called from tc interrupt
/// FIXME: this is overly generalized for our purpose, i think.
void process_timers( void ) {
  int k;
  swTimer_t* t;

  // Process the timer list 
  for ( k = 0; k < MAX_TIMERS; k++ ) {
    // Skip unused timer slots...
    t = timers[k];
    if ( t == 0 ) { continue; }

    if ( t->timeout > 0 ) {
      // time remaining, decrement tick counter
      t->timeout--;

      // if timer expired, call the callback function
      if ( t->timeout <= 0 ) {
	if ( t->callback != 0 ) {
	  (*t->callback)( t->tag );
	}

	// if periodic, reload the timer
	if ( t->fperiodic ) {
	  t->timeout = t->timeoutReload;
	} else {
	  // delete non-periodic timerx
	  timers[k] = NULL;
	}
      }
    }
  }
}

