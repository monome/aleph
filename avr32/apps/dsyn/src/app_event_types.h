#ifndef _APP_DSYN_EVENT_TYPES_H_
#define _APP_DSYN_EVENT_TYPES_H_

// application-sepecific event types

// FIXME: this was just tacked on so the nomenclature is odd.

typedef enum {
  // trigger advnacement to the next sequencer stage
  kEventSeqNext = kNumEvents,
  kNumAppEvents
  
} eAppEventType;

#endif
