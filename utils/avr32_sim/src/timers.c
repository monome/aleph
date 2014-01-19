/* timers.c
 * aleph-avr32
 *
 * ultra-simple software timers.
 */

// asf
#include "print_funcs.h"
// aleph-avr32
//#include "conf_tc_irq.h"
#include "timers.h"

//-----------------------------------------------
//---- static variables

// store timers in a simple one-way linked list.
// FIXME: remove() would be faster with two-way list.
// top of list
softTimer_t* top = NULL;
// size of list
u32 num = 0;

//------------------------------
//--- extern functions

void init_timers(void) {
#if 1
#else
  ;; // nothing to do
#endif
}

// set a periodic timer with a callback
// return 1 if set, 0 if not
u8 timer_add( softTimer_t* t, u32 ticks, timer_callback_t callback, void* obj) {
#if 1
#else
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
    t->caller = obj;
    cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
    return 1;
  } else {
    // timer was already set, do nothing
    return 0;
  }
#endif
}

// find and remove a timer from the processing list
// return 1 if removed, 0 if not found
u8 timer_remove( softTimer_t* t) {
#if 1
#else
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
#endif
}

// process the timer list, presumably from TC interrupt
void process_timers( void ) {
#if 1
#else
  u32 i;
  softTimer_t* t = top;
  for(i = 0; i<num; ++i) {
    --(t->ticksRemain);
    if(t->ticksRemain == 0) {
      (*(t->callback))(t->caller);   
      t->ticksRemain = t->ticks;
    }
    t = t->next;
  }
#endif
}

