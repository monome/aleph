#ifndef _APP_DSYN_EVENT_TYPES_H_
#define _APP_DSYN_EVENT_TYPES_H_

// application-sepecific event types

typedef enum {
  // trigger advnacement to the next sequencer stage
  kEventSeqNext = kNumSysEvents,
  kNumAppEvents
  
} eAppEventType;

#endif
