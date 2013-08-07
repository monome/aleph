/* events.c
 * aleph
 *
 * simple event queue
 * disables interrupts around queue manipulation
 */

// ASF
#include "compiler.h"
#include "print_funcs.h"

// aleph-avr32
#include "aleph_board.h"
#include "conf_tc_irq.h"
#include "events.h"
#include "event_types.h"


/// NOTE: if we are ever over-filling the event queue, we have problems.
/// making the event queue bigger not likely to solve the problems.
#define MAX_EVENTS   64

// macro for incrementing an index into a circular buffer.
#define INCR_EVENT_INDEX( x )  { if ( ++x == MAX_EVENTS ) x = 0; }

// et/Put indexes inxto sysEvents[] array
static int putIdx = 0;
static int getIdx = 0;

// The system event queue is a circular array of event records.
static event_t sysEvents[ MAX_EVENTS ];

// initializes (or re-initializes)  the system event queue.
void init_events( void ) {
  int k;
  
  // set queue (circular list) to empty
  putIdx = 0;
  getIdx = 0;

  // zero out the event records
  for ( k = 0; k < MAX_EVENTS; k++ ) {
    sysEvents[ k ].eventType = 0;
    sysEvents[ k ].eventData = 0;
  }
}

// get next event
// Returns non-zero if an event was available
bool get_next_event( event_t *e ) {
  bool status;
  //  bool fReenableInterrupts = Is_interrupt_level_enabled( TIMER_INT_LEVEL );
  //  Disable_interrupt_level( TIMER_INT_LEVEL );
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  
  // if pointers are equal, the queue is empty... don't allow idx's to wrap!
  if ( getIdx != putIdx ) {
    INCR_EVENT_INDEX( getIdx );
    e->eventType = sysEvents[ getIdx ].eventType;
    e->eventData = sysEvents[ getIdx ].eventData;
    status = true;
  } else {
    e->eventType  = 0xff;
    e->eventData = 0;
    status = false;
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  //  if (fReenableInterrupts) {
    //    Enable_interrupt_level( TIMER_INT_LEVEL );
  //  }
  return status;
}


// add event to queue, return success status
bool post_event( event_t *e ) {
  bool status = false;
  int saveIndex;

  //  print_dbg("\r\n posting event, type: ");
  //  print_dbg_ulong(e->eventType);


  //  bool fReenableInterrupts = Is_interrupt_level_enabled( TIMER_INT_LEVEL );
  //  Disable_interrupt_level( TIMER_INT_LEVEL );
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  
  // increment write idx, posbily wrapping
  saveIndex = putIdx;
  INCR_EVENT_INDEX( putIdx );
  if ( putIdx != getIdx  ) {
    sysEvents[ putIdx ].eventType = e->eventType;
    sysEvents[ putIdx ].eventData = e->eventData;
    status = true;
  } else {
    // idx wrapped, so queue is full, restore idx
    putIdx = saveIndex;
    print_dbg("\r\n event queue full!");
  } 

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  //  if (fReenableInterrupts) {
  //    Enable_interrupt_level( TIMER_INT_LEVEL );
  //  }

  return status;
}



//////////////////////////////////////////////////////////
/// testing
//u32 get_max_events(void) { return MAX_EVENTS; }
//extern event_t* get_sys_events(void) { return (event_t*)sysEvents; }

void print_pending_events(void) {
  u32 i;
  i = getIdx;

  while(i != putIdx) {
    print_dbg("\r\n  pending system events:");
    
    print_dbg("\r\v event at idx ");
    print_dbg_ulong(i);
    print_dbg(" type: ");
    print_dbg_ulong( sysEvents[i].eventType);
    print_dbg(" data: 0x");
    print_dbg_hex(sysEvents[i].eventType);

    INCR_EVENT_INDEX(i);
  }
}

//////////////////////////////
