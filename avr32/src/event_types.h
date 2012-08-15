/* event_types.h
 * aleph
 *
 * enumeration of application events
 */

#ifndef _EVENT_TYPES_H_
#define _EVENT_TYPES_H_

typedef enum {
  kEventSwitchDown0 = 0,
  kEventSwitchUp0 = 1,
  kEventSwitchDown1 = 2,
  kEventSwitchUp1 = 3,
  kEventSwitchDown2 = 4,
  kEventSwitchUp2 = 5,
  kEventSwitchDown3 = 6,
  kEventSwitchUp3 = 7,
  //kEventSwitchDown4,
  //kEventSwitchUp4,
  //kEventSwitchDown5,
  //kEventSwitchUp5,
  kEventEncoder0 = 8,
  kEventEncoder1 = 9,
  kEventEncoder2 = 10,
  kEventEncoder3 = 11,
  kEventAdc0 = 12,
  kEventAdc1 = 13,
  kEventAdc2 = 14,
  kEventAdc3 = 15,
  kEventRefresh = 16,
  kNumEvents = 17
} eEventType;

#endif
