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
#include "screen.h"


/// NOTE: if we are ever over-filling the event queue, we have problems.
/// making the event queue bigger not likely to solve the problems.
#define MAX_EVENTS   32

// keep track of how many events we've rejected,
// and allow the queue to wrap after some number
#define MAX_EVENT_FULL_COUNT 2
static int fullCount = 0;


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
    sysEvents[ k ].type = 0;
    sysEvents[ k ].data = 0;
  }
}

// get next event
// Returns non-zero if an event was available
u8 event_next( event_t *e ) {
  u8 status;
  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  
  // if pointers are equal, the queue is empty... don't allow idx's to wrap!
  if ( getIdx != putIdx ) {
    INCR_EVENT_INDEX( getIdx );
    e->type = sysEvents[ getIdx ].type;
    e->data = sysEvents[ getIdx ].data;
    status = true;
  } else {
    e->type  = 0xff;
    e->data = 0;
    status = false;
  }

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  return status;
}


static inline void write_event(event_t* e) { 
  sysEvents[ putIdx ].type = e->type;
  sysEvents[ putIdx ].data = e->data;
  fullCount = 0;
}

// add event to queue, return success status
u8 event_post( event_t *e ) {
  u8 status = false;
  int saveIndex;

  cpu_irq_disable_level(APP_TC_IRQ_PRIORITY);
  
  // increment write idx, posbily wrapping
  saveIndex = putIdx;
  INCR_EVENT_INDEX( putIdx );
  if ( putIdx != getIdx  ) {
    write_event(e);
    status = true;
  } else {
    // if queue stays full for too long, let it wrap
    if(fullCount++ < MAX_EVENT_FULL_COUNT) { 
      putIdx = saveIndex;
      print_dbg("\r\n event queue full!");
    } else {
      write_event(e);
      status = true;
    }
    // show the user that bad things are happening
    screen_flash();
  } 

  cpu_irq_enable_level(APP_TC_IRQ_PRIORITY);
  return status;
}



//////////////////////////////////////////////////////////
/// testing
//u32 get_max_events(void) { return MAX_EVENTS; }
//extern event_t* get_sys_events(void) { return (event_t*)sysEvents; }
/*
void print_pending_events(void) {
  u32 i;
  i = getIdx;

  while(i != putIdx) {
    print_dbg("\r\n  pending system events:");
    
    print_dbg("\r\v event at idx ");
    print_dbg_ulong(i);
    print_dbg(" type: ");
    print_dbg_ulong( sysEvents[i].type);
    print_dbg(" data: 0x");
    print_dbg_hex(sysEvents[i].type);

    INCR_EVENT_INDEX(i);
  }
}
*/
//////////////////////////////
