/* events.c
 * aleph
 *
 * simple event queue
 * disables interrupts around queue manipulation
 */

#include "compiler.h"
#include "timers.h" // for timer interrupt priority level
#include "events.h"

#define MAX_EVENTS   32

// macro for incrementing an index into a circular buffer.
#define INCR_EVENT_INDEX( x )  { if ( ++x == MAX_EVENTS ) x = 0; }

// et/Put indexes into sysEvents[] array
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
  bool status = false;
  bool fReenableInterrupts = Is_interrupt_level_enabled( TIMER_INT_LEVEL );

  Disable_interrupt_level( TIMER_INT_LEVEL );
  
  // if pointers are equal, the queue is empty... don't allow idx's to wrap!
  if ( getIdx != putIdx ) {
    INCR_EVENT_INDEX( getIdx );
    e->eventType = sysEvents[ getIdx ].eventType;
    e->eventData = sysEvents[ getIdx ].eventData;
    status = true;
  } else {
    e->eventType  = 0xff;
    e->eventData = 0;
  }

  if (fReenableInterrupts) {
    Enable_interrupt_level( TIMER_INT_LEVEL );
  }

  return status;
}


// add event to queue, return success status
bool post_event( event_t *e ) {
  bool status = false;
  int saveIndex;
  bool fReenableInterrupts = Is_interrupt_level_enabled( TIMER_INT_LEVEL );

  Disable_interrupt_level( TIMER_INT_LEVEL );

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
  }
  
  if (fReenableInterrupts) {
    Enable_interrupt_level( TIMER_INT_LEVEL );
  }

  return status;
}
