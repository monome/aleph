/* event_types.h
 * aleph
 *
 * enumeration of application events
 */

#ifndef _EVENT_TYPES_H_
#define _EVENT_TYPES_H_

typedef enum {
  ///------ periodic system events:
  /// ADC polling
  kEventAdc0 ,
  kEventAdc1 ,
  kEventAdc2 ,
  kEventAdc3 ,
  /// screen refresh
  kEventRefresh,
  /// monome polling
  kEventMonomeRead,
  /// midi polling
  //...
  /// HID polling
  //...
  ///------ discrete system events
  /// connect/disconnect usb devices
  kEventFtdiConnect,
  kEventFtdiDisconnect,
  ///------ discrete UI events
  /// encoders
  kEventEncoder0,
  kEventEncoder1,
  kEventEncoder2 ,
  kEventEncoder3 ,
  //// switches
  kEventSwitchDown0, // edit
  kEventSwitchUp0,
  kEventSwitchDown1, 
  kEventSwitchUp1,
  kEventSwitchDown2,
  kEventSwitchUp2,
  kEventSwitchDown3,
  kEventSwitchUp3,
  kEventSwitchDown4, // mode
  kEventSwitchUp4,
  kEventSwitchDown5, // power
  kEventSwitchUp5,
  kEventSwitchDown6, // foot0
  kEventSwitchUp6,
  kEventSwitchDown7, // foot1
  kEventSwitchUp7,
  kNumEvents,
} eEventType;

#endif
