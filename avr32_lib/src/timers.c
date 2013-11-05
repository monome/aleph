/* timers.c
 * aleph-avr32
 *
 * ultra-simple software timers.
 */

// asf
#include "print_funcs.h"
// aleph-avr32
#include "conf_tc_irq.h"
#include "timers.h"

//-----------------------------------------------
//---- static variables

// use a simple one-way linked list.
// not really worth declaring a class for this...

// top of list
softTimer_t* top = NULL;
u32 num = 0;

//------------------------------
//--- extern functions

void init_timers(void) {
  ;; // nothing to do
}

// set a periodic timer with a callback
// return 1 if set, 0 if not
u8 timer_add( softTimer_t* t, u32 ticks, timer_callback_t callback) {
  if (ticks == 0) { 
    ticks = 1;
  }
  if( t->next == NULL) {
    // disable timer interrupts
    cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);

    // this timer is unset
    if ( top == NULL ) {
      if (num == 0) {
	// timer list is empty, so make this the top of the list
	top = t;
	num = 1;
	// set this just so we know it's in the list.
	t->next = t;
      } else {
	// shouldn't get here!
	print_dbg("\r\n insanity in timer list! nonzero count of timers but no top element.");
      }
    } else {
      // push this timer to the top of the list
      t->next = top;
      top = t;
      ++num;
    }
    t->callback = callback;
    t->ticksRemain = ticks;
    t->ticks = ticks;

    cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
    return 1;
  } else {
    // timer was already set, do nothing
    return 0;
  }
}

// find and remove a timer from the processing list
// return 1 if removed, 0 if not found
u8 timer_remove( softTimer_t* t) {
  softTimer_t* cur = top;
  softTimer_t* last = NULL;
  u32 i;
  u8 ret = 0;
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  // search for the timer in the linked list
  for(i=0; i<num; ++i) {
    if(cur == t) {
      // found it...
      if(last == NULL) {
	// target timer is probably at the top.
	if(top != t) { // sanity check
	  print_dbg("\r\n warning! timer list is insane. non-top element has no parent.");
	  ret = 0; break;
	}
	if(t->next == t) {
	  // target timer is the top element, and also the last.
	  // empty the list, unlink, get out
	  top = NULL;
	  t->next = NULL;
	  num = 0;
	  ret = 1; break;
	} else {
	  // target is top element, and there are more in the list.
	  // make the next element into the top, unlink, get out
	  top = t->next;
	  t->next = NULL;
	  --num;
	  ret = 1; break;
	}
      } else {
	// target timer is not at the top.
	if(last->next != t) { // sanity check
	  print_dbg("\r\n warning! timer list is insane; link mismatch.");
	  ret = 0; break;
	}
	// made it here, so everything is sane, and this is not the top timer.
	// unlink and report success
	last->next = t->next;
	t->next = NULL;
	--num;
      }
    }
    // made it here, so we didn't find it; advance the search.
    last = cur;
    cur = cur->next;
  } // end of search loop
  
  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  return ret;
}

// process the timer list, presumably from TC interrupt
void process_timers( void ) {
  u32 i;
  softTimer_t* t = top;
  for(i = 0; i<num; ++i) {
    --(t->ticksRemain);
    if(t->ticksRemain == 0) {
      (*(t->callback))();   
      t->ticksRemain = t->ticks;
    }
    t = t->next;
  }
}




/* ///////////////////// */
/* ///////////////// */
/* ///////////// */
/* #if 0 */

/* // pointers to timers */
/* static swTimer_t*  timers[ MAX_TIMERS ]; */

/* //----------------------------------------------- */
/* //---- static functions */

/* static swTimer_t* find_timer( timerCallback callback, int tag ); */
/* static bool add_timer( swTimer_t* newTimer ); */

/* // find a timer given a tag and optional callback */
/* static swTimer_t* find_timer( timerCallback callback, int tag ) { */
/*   int k; */
/*   swTimer_t* t; */

/*   //  bool fReenableInterrupts = Is_interrupt_level_enabled( TIMER_INT_LEVEL ); */
/*   //  Disable_interrupt_level( TIMER_INT_LEVEL ); */
/*   cpu_irq_disable_level(APP_TC_IRQ_PRIORITY); */

/*   for ( k = 0; k < MAX_TIMERS; k++ ) { */
/*     t = timers[k]; */
/*     if ( t == NULL ) { */
/*       continue; */
/*     } */

/*     if ( t->tag == tag ) { */
/*       if ( callback != NULL ) { */
/* 	if ( t->callback == callback ) { */
/* 	  // tag and callback both specified and matched */
/* 	  //	  if (fReenableInterrupts) */
/* 	  //	    Enable_interrupt_level( TIMER_INT_LEVEL ); */
/* 	  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */
/* 	  return t; */
/* 	} else { */
/* 	  // tag and callback specified, but only tag matched */
/* 	  continue; */
/* 	} */
/*       } */

/*       // only tag specified, matched */
/*       //      if (fReenableInterrupts) { */
/*       //	Enable_interrupt_level( TIMER_INT_LEVEL );  */
/*       cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */
/*       return t; */
/* 	//      } */
/*     } */
/*   } */

/*   cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */
/*   /\* if (fReenableInterrupts) { *\/ */
/*   /\*   Enable_interrupt_level( TIMER_INT_LEVEL ); *\/ */
/*   /\* } *\/ */

/*   return NULL; */
/* } */

/* // Add timer to pointer array. Finds first empty slot. */
/* static bool add_timer( swTimer_t* newTimer) { */
/*   int k; */
/*   //  bool fReenableInterrupts = Is_interrupt_level_enabled( TIMER_INT_LEVEL ); */
/*   //  Disable_interrupt_level( TIMER_INT_LEVEL ); */
/*   cpu_irq_disable_level(APP_TC_IRQ_PRIORITY); */
  
/*   // find empty slot */
/*   for ( k = 0; k < MAX_TIMERS; k++ ) { */
/*     if ( timers[k] ==  NULL ) { */
/*       timers[k] = newTimer; */
/*       cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */
/*       //      Enable_interrupt_level( TIMER_INT_LEVEL ); */
/*       return true; */
/*     } */
/*   } */

/*   cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */
/*   //  if (fReenableInterrupts) { */
/*   //    Enable_interrupt_level( TIMER_INT_LEVEL ); */
/*   //  } */

/*   return false; */
/* } */

/* //----------------------------------------------- */
/* //---- external functions */

/* // initialize the timer system. */
/* void init_timers( void ) { */
/*   int k; */

/*   for ( k = 0; k < MAX_TIMERS; k++ ) { */
/*     timers[ k ] = 0; */
/*   } */
/* } */

/* // Add a callback timer to the list. */
/* bool set_timer(  swTimer_t* t, int tag, int ticks, timerCallback callback, */
/* 		 bool fPeriodic ) { */
/*   if ( callback == NULL ) { */
/*     return false; */
/*   } */
/*   // dont add if callback appears in the list */
/*   if ( find_timer( callback, tag ) != NULL ) { */
/*     return false; */
/*   } */

/*   t->callback = callback; */
/*   t->tag = tag; */
/*   t->timeout = ticks; */
/*   t->fperiodic = fPeriodic; */
/*   t->timeoutReload = ticks; */

/*   return add_timer( t ); */
/* } */

/* // kill a timer given its tag value. */
/* bool kill_timer( int tag ) { */
/*   int k; */
  
/*   cpu_irq_disable_level(APP_TC_IRQ_PRIORITY); */

/*   for ( k = 0; k < MAX_TIMERS; k++ ) { */
/*     if ( timers[k]->tag == tag ) { */
/*       timers[k] = NULL; */
/*       cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */
/*       return true; */
/*     } */
/*   } */

/*   cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */

/*   return false; */
/* } */


/* // process the list of timers. */
/* // called from tc interrupt */
/* /// FIXME: this is overly generalized for our purpose, i think. */
/* void process_timers( void ) { */
/*   int k; */
/*   swTimer_t* t; */

/*   // Process the timer list  */
/*   for ( k = 0; k < MAX_TIMERS; k++ ) { */
/*     // Skip unused timer slots... */
/*     t = timers[k]; */
/*     if ( t == 0 ) { continue; } */

/*     if ( t->timeout > 0 ) { */
/*       // time remaining, decrement tick counter */
/*       t->timeout--; */

/*       // if timer expired, call the callback function */
/*       if ( t->timeout <= 0 ) { */
/* 	if ( t->callback != 0 ) { */
/* 	  (*t->callback)( t->tag ); */
/* 	} */

/* 	// if periodic, reload the timer */
/* 	if ( t->fperiodic ) { */
/* 	  t->timeout = t->timeoutReload; */
/* 	} else { */
/* 	  // delete non-periodic timerx */
/* 	  timers[k] = NULL; */
/* 	} */
/*       } */
/*     } */
/*   } */
/* } */

/* #endif */
