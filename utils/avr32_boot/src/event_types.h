/* event_types.h
 * aleph
 *
 * enumeration of application events
 */

#ifndef _EVENT_TYPES_H_
#define _EVENT_TYPES_H_

/// BE CAREFUL changing these as the order needs to be somewhat specific.
/// basically, i'm putting the most frequent events first,
/// for speed of switch statements,
/// this also allows for some ugly comparison hacks.
typedef enum {
  ///------ periodic system events:
  /// first, ADC polling
  kEventAdc0 ,
  kEventAdc1 ,
  kEventAdc2 ,
  kEventAdc3 ,
  /// next, screen refresh
  kEventRefresh,
  ///------ discrete UI events
  /// encoders
  kEventEncoder0,
  kEventEncoder1,
  kEventEncoder2 ,
  kEventEncoder3 ,
  //// switches
  kEventSwitchDown0, // edit
  kEventSwitchUp0,
  // kEventSwitchDown1, 
  // kEventSwitchUp1,
  // kEventSwitchDown2,
  // kEventSwitchUp2,
  // kEventSwitchDown3,
  // kEventSwitchUp3,
  // kEventSwitchDown4, // mode
  // kEventSwitchUp4,
  // kEventSwitchDown5, // power
  // kEventSwitchUp5,
  // kEventSwitchDown6, // foot0
  // kEventSwitchUp6,
  // kEventSwitchDown7, // foot1
  // kEventSwitchUp7,
  kNumEvents,
} eEventType;

#endif
