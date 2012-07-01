/* events.h
 * aleph
 */

#ifndef _EVENTS_H_
#define _EVENTS_H_

#include <compiler.h>

typedef struct {
  U32   eventType;
  S32	eventData;
} event_t;


/* Initializes (or re-initializes)  the system event queue.
 */
void init_events( void );


/* Returns true if there's an event available, otherwise returns
 * false if there's nothing to do. The event record provided is filled
 * in with a copy of the event data.
 */
bool get_next_event( event_t *e );


/* Add an event to the system event queue. Values in the provided
 * event record are copied into the system event queue. Returns
 * true if the supplied event was successfully posted, else false.
 */
bool post_event( event_t *e );


#endif // header guard
