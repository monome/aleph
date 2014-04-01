/* timers.c
 * aleph-avr32
 *
 * ultra-simple software timers.
 */

// asf
#include "print_funcs.h"
#include "gpio.h"
// aleph-avr32
#include "conf_tc_irq.h"
#include "timers.h"

//-----------------------------------------------
//---- static variables


// 2-way linked list of timers
static volatile softTimer_t* head = NULL;
static volatile softTimer_t* tail = NULL;
static volatile u32 num = 0;

//------------------------------
//--- extern functions

void init_timers(void) {
  ;; // nothing to do
}

// set a periodic timer with a callback
// return 1 if set, 0 if not
/* u8 timer_add( softTimer_t* t, u32 ticks, timer_callback_t callback, void* obj) { */
/*   s32 ret; */

/*   /// tEST */
/*   //  gpio_clr_gpio_pin(LED_MODE_PIN); */
/*   //// */

/*   if (ticks == 0) {  */
/*     ticks = 1; */
/*   } */
/*   if( t->next == NULL) { */
/*     // disable timer interrupts */
/*     cpu_irq_disable_level(APP_TC_IRQ_PRIORITY); */

/*     // head timer is unset */
/*     if ( head == NULL ) { */
/*       if (num == 0) { */
/* 	// timer list is empty, so make this the head of the list */
/* 	head = t; */
/* 	num = 1; */
/* 	// set this just so we know it's in the list. */
/* 	t->next = t; */
/*       } else { */
/* 	// shouldn't get here! */
/* 	// print_dbg("\r\n insanity in timer list! nonzero count of timers but no head element."); */
/*       } */
/*     } else { */
/*       // push this timer to the head of the list */
/*       t->next = head; */
/*       head = t; */
/*       ++num; */
/*     } */
/*     t->callback = callback; */
/*     t->ticksRemain = ticks; */
/*     t->ticks = ticks; */
/*     t->caller = obj; */
/*     cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */
/*     ret = 1; */
/*   } else { */
/*     // timer was already set, do nothing */
/*     ret = 0; */
/*   } */

  
/*   /// tEST */
/*   // gpio_set_gpio_pin(LED_MODE_PIN); */
/*   //// */

/*   return ret; */

/* } */

/* // find and remove a timer from the processing list */
/* // return 1 if removed, 0 if not found */
/* u8 timer_remove( softTimer_t* t) { */
/*   volatile softTimer_t* cur = head; */
/*   volatile softTimer_t* last = NULL; */
/*   u32 i; */
/*   u8 ret = 0; */

/*   /// tEST */
/*   gpio_clr_gpio_pin(LED_MODE_PIN); */


/*   // disable timer interrupts */
/*   cpu_irq_disable_level(APP_TC_IRQ_PRIORITY); */

/*   // search for the timer in the linked list */
/*   // print_dbg("\r\n unsetting timer, search start"); */
/*   for(i=0; i<num; ++i) { */
/*     if(cur == t) { */
/*       // found it... */
/*       // print_dbg("\r\n unsetting timer, found at idx: "); */
/*       // print_dbg_ulong(i); */
/*       // print_dbg(", address: 0x"); */
/*       // print_dbg_hex((u32)cur); */
/*       if(last == NULL) { */
/* 	// target timer is probably at the head. */
/* 	if(head != t) { // sanity check */
/* 	  // print_dbg("\r\n warning! timer list is insane. non-head element has no parent."); */
/* 	  ret = 0; break; */
/* 	} */
/* 	if(t->next == t) { */
	  
/* 	  // print_dbg(", timer is head element and last element."); */
/* 	  // target timer is the head element, and also the last. */
/* 	  // empty the list, unlink, get out */
/* 	  head = NULL; */
/* 	  t->next = NULL; */
/* 	  num = 0; */
/* 	  ret = 1; break; */
/* 	} else { */
/* 	  // print_dbg(", timer is head element, not last element."); */
/* 	  // target is head element, and there are more in the list. */
/* 	  // make the next element into the head, unlink, get out */
/* 	  head = t->next; */
/* 	  t->next = NULL; */
/* 	  --num; */
/* 	  ret = 1; break; */
/* 	} */
/*       } else { // last != NULL */
/* 	// target timer is not at the head. */
/* 	if(last->next != t) { // sanity check */
/* 	  // print_dbg("\r\n warning! timer list is insane; link mismatch."); */
/* 	  ret = 0; break; */
/* 	} */
/* 	// made it here, so everything is sane, and this is not the head timer. */
/* 	// unlink and report success */
	
/* 	// print_dbg(", timer is not the head element; next element address: 0x"); */
/* 	// print_dbg_hex((u32)(t->next)); */
/* 	// print_dbg(", last element address: 0x"); */
/* 	// print_dbg_hex((u32)(last->next)); */
/* 	if(last != NULL) { */
/* 	  last->next = t->next; */
/* 	} */
/* 	t->next = NULL; */
/* 	--num; */
/*       } */
/*     } */
/*     // print_dbg(", advance search... "); */
/*     // made it here, so we didn't find it; advance the search. */
/*     last = cur; */
/*     cur = cur->next; */
/*   } // end of search loop */
  
/*   cpu_irq_enable_level(APP_TC_IRQ_PRIORITY); */

/*   /// tEST */
/*   gpio_set_gpio_pin(LED_MODE_PIN); */


/*   return ret; */
/* } */


// set a periodic timer with a callback
// return 1 if set, 0 if not
u8 timer_add( softTimer_t* t, u32 ticks, timer_callback_t callback, void* obj) {
  //  int i;
  int ret;

  // disable timer interrupts
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);

  print_dbg("\r\n timer_add, @ 0x");
  print_dbg_hex((u32)t);

  if(t->prev == NULL || t->next == NULL) {
    print_dbg(" ; timer is unlinked ");
    // is list empty?
    if( (head == NULL) || (tail == NULL)) {
      print_dbg(" ; list was empty ");
      head = tail = t;
      t->next = t->prev = t;
      num = 1;
      print_dbg(" ; added timer as sole element ");

    } else {
      // list not empty, add to tail
      tail->next = t;
      head->prev = t;
      t->prev = tail;
      t->next = head;
      tail = t;
      ++num; 

    } 
    t->callback = callback; 
    t->caller = obj;
    if(ticks < 1) { ticks = 1; }
    t->ticksRemain = ticks;
    t->ticks = ticks;
    ret = 1;
    print_dbg(" ; added timer to tail ; new count: ");
    print_dbg_ulong(num);
  } else {
    print_dbg(" ; timer was already linked, aborting ");
    ret = 0;
  }

  // enable timer interrupts
  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  return ret;
}

// remove a timer from the list
// return 1 if removed, 0 if not found
u8 timer_remove( softTimer_t* t) {
  int i;
  volatile softTimer_t* pt = NULL;
  u8 found = 0;

  // disable timer interrupts
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);


  // not linked
  if( (t->next == NULL) || (t->prev == NULL)) { return 0; }

  // check head
  if(t == head) { 
    found = 1;
    head = t->next;
  }
  // check tail
  else if(t == tail) { 
    found = 1;
    tail = t->prev; 
  } else {
    // search 
    pt = head;
    for(i=0; i<num; ++i) {
      if(pt == t) {
	// found it
	found = 1;
	break;
      }
      pt = pt->next;
    }
  }
  if(found) {
    // unlink and decrement
    (t->next)->prev = t->prev;
    (t->prev)->next = t->next;
    t->next = t->prev = 0;
    --num;
  }

  // enable timer interrupts
  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  return found;
}


// clear the list
//// ???? ???
 void timers_clear(void) {
   int i;
   volatile softTimer_t* pt;
   if(head != NULL) {
     print_dbg("\r\n clearing timer list, size: ");
     print_dbg_ulong(num);

     pt = head;
     // ??? will it work ???
     for(i=0; i<num; ++i) {       
       pt->prev = NULL;
       pt = pt->next;
       pt->prev->next = NULL;
     }	 
   }
   head = NULL;
   tail = NULL;
   num = 0;
}

// process the timer list, presumably from TC interrupt
void process_timers( void ) {
  u32 i;
  volatile softTimer_t* t = head;

  //  print_dbg("\r\n processing timers. head: 0x");
  //  print_dbg_hex((u32)head);

  // ... important...  ?
  if ( (head == NULL) || (tail == NULL) || (num == 0) ) { 
    //    print_dbg("\r\n processing empty timer list");
    return; 
  }

  for(i = 0; i<num; ++i) {
    --(t->ticksRemain);
    if(t->ticksRemain == 0) {
      (*(t->callback))(t->caller);   
      t->ticksRemain = t->ticks;
      //      print_dbg("\r\n triggered timer callback @ 0x");
      //      print_dbg_hex((u32)t);
    }
    t = t->next;
    
    //    print_dbg("; advanced list pointer, now: 0x");
    //    print_dbg_hex((u32)t);
  }
}

