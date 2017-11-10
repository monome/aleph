/* 
   net_midi.h

   midi management routines for the bees control network.
   handles operator focus, routes handler/refresh requests

*/

#ifndef _ALEPH_BEES_NET_MIDI_H_
#define _ALEPH_BEES_NET_MIDI_H_


#include "types.h"
#include "util.h"
#ifndef BEEKEEP
#include "midi.h"
#include "midi_common.h"
#endif

EXTERN_C_BEGIN



//-----------------------
//---- types
// abstract superclass for midi operators
// has event handler and focus flag
typedef void (*net_midi_note_on_t)(void *op, u8 ch, u8 num, u8 vel);
typedef void (*net_midi_note_off_t)(void *op, u8 ch, u8 num, u8 vel);
typedef void (*net_midi_channel_pressure_t)(void *op, u8 ch, u8 val);
typedef void (*net_midi_pitch_bend_t)(void *op, u8 ch, u16 bend);
typedef void (*net_midi_control_change_t)(void *op, u8 ch, u8 num, u8 val);
typedef void (*net_midi_program_change_t)(void *op, u8 ch, u8 num);
typedef void (*net_midi_real_time_t)(void *op);
typedef void (*net_midi_panic_t)(void *op);

typedef struct {
  net_midi_note_on_t note_on;
  net_midi_note_off_t note_off;
  net_midi_channel_pressure_t channel_pressure;
  net_midi_pitch_bend_t pitch_bend;
  net_midi_control_change_t control_change;
  net_midi_program_change_t program_change;

  net_midi_real_time_t clock_tick;
  net_midi_real_time_t seq_start;
  net_midi_real_time_t seq_stop;
  net_midi_real_time_t seq_continue;

  net_midi_panic_t panic;
} net_midi_behavior_t;

typedef struct _op_midi {
  // handler function, will connect to app event handler
  net_midi_behavior_t handler;
  // pointer to operator subclass
  void* sub;
  // pointers to adjacent list elements
  struct _op_midi* next;
} op_midi_t;
//-------------------------
//--- extern functions


extern void net_midi_init(op_midi_t *m);
extern void net_midi_note_subscribe(op_midi_t *s);
extern void net_midi_note_unsubscribe (op_midi_t *u);
extern void net_midi_control_change_subscribe(op_midi_t *s);
extern void net_midi_control_change_unsubscribe (op_midi_t *u);
extern void net_midi_program_change_subscribe(op_midi_t *s);
extern void net_midi_program_change_unsubscribe (op_midi_t *u);
extern void net_midi_real_time_subscribe(op_midi_t *s);
extern void net_midi_real_time_unsubscribe (op_midi_t *u);

// add op pointer after the current position
//extern void net_midi_list_add(op_midi_t* op);
extern void net_midi_list_push(op_midi_t* op);
// delete op pointer at current position, return next pointer in list
extern void net_midi_list_remove(op_midi_t* op);
// handle incoming midi packet
extern void net_handle_midi_packet(u32 data);

EXTERN_C_END
#endif // h guard
