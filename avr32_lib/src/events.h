/* events.h
 * aleph
 */

#ifndef _EVENTS_H_
#define _EVENTS_H_

#include "event_types.h"
#include "types.h"

typedef struct {
  etype type;
  s32 data;
} event_t;


// init event queue
void init_events( void );


// check the queue for pending events
// return 1 if found
u8 event_next( event_t *e );


// add event to tail of queue
// return 1 if success
u8 event_post( event_t *e );

#endif // header guard
