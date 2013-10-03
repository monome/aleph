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
  /// poll monome
  kEventMonomePoll,
  // refresh monome
  kEventMonomeRefresh,
  // handle rx data

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
  /* kEventEncoder0Start, */
  /* kEventEncoder0Move, */
  /* kEventEncoder0End, */
  /* kEventEncoder1Start, */
  /* kEventEncoder1Move, */
  /* kEventEncoder1End, */
  /* kEventEncoder2Start, */
  /* kEventEncoder2Move, */
  /* kEventEncoder2End, */
  /* kEventEncoder3Start, */
  /* kEventEncoder3Move, */
  /* kEventEncoder3End, */
  kEventEncoder0,
  kEventEncoder1,
  kEventEncoder2 ,
  kEventEncoder3 ,
  //// switches
  kEventSwitch0, // fn1
  kEventSwitch1, // fn2
  kEventSwitch2, // fn3
  kEventSwitch3, // fn4
  kEventSwitch4, // mode
  kEventSwitch5, // power
  kEventSwitch6, // foot1
  kEventSwitch7, // foot2
  ///// monome input
  kEventMonomeGridKey,
  kEventMonomeGridTilt,
  kEventMonomeRingEnc,
  kEventMonomeRingKey,
  //// monome connect / disconnect
  kEventMonomeConnect,
  kEventMonomeDisconnect,

  /// MIDI

  /// HID
  kEventHidByte,
  /// dummy/count
  kNumSysEvents,
} eEventType;

#endif
