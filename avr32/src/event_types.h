/* event_types.h
 * aleph
 *
 * enumeration of application events
 */

#ifndef _EVENT_TYPES_H_
#define _EVENT_TYPES_H_

// enumerate event types
// WARNING: changing layout of this could break existing avr32 applications.. :S
typedef enum {
  kEventAdc0 , 	// receive values from polled ADC channels
  kEventAdc1 ,
  kEventAdc2 ,
  kEventAdc3 ,
  /// encoders
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
  /// connect/disconnect FTDI-based devices
  kEventFtdiConnect,
  kEventFtdiDisconnect,
  /// monome protocol
  kEventMonomeConnect,
  kEventMonomeDisconnect,
  kEventMonomePoll,	
  kEventMonomeRefresh, 	
  kEventMonomeGridKey, 
  kEventMonomeGridTilt,
  kEventMonomeRingEnc,
  kEventMonomeRingKey,
  //// MIDI class
  kEventMidiConnect,
  kEventMidiDisconnect,
  kEventMidiPacket,
  kEventMidiRefresh,
  /// HID class
  kEventHidConnect,
  kEventHidDisconnect,
  kEventHidByte,
  /// dummy/count
  kNumSysEvents,
} eEventType;

#endif
