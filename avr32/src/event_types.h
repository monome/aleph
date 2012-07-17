/* event_types.h
 * aleph
 *
 * enumeration of application events
 */

#ifndef _EVENT_TYPES_H_
#define _EVENT_TYPES_H_

typedef enum {
  kEventSwitchDown0,
  kEventSwitchUp0,
  kEventSwitchDown1,
  kEventSwitchUp1,
  kEventSwitchDown2,
  kEventSwitchUp2,
  kEventSwitchDown3,
  kEventSwitchUp3,
  kEventEncoder0,
  kEventEncoder1,
  kEventEncoder2,
  kEventEncoder3,
  kEventAdc0,
  kEventAdc1,
  kEventAdc2,
  kEventAdc3,
  kEventRefresh,
  kNumEvents
} eEventType;

#endif
