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
typedef struct _op_midi {
  // handler function, will connect to app event handler
  void *handler;
  // pointer to operator subclass
  void* sub;
  // pointers to adjacent list elements
  struct _op_midi* next;
} op_midi_t;
//-------------------------
//--- extern functions


extern void net_midi_note_subscribe(op_midi_t *s);
extern void net_midi_note_unsubscribe (op_midi_t *u);
extern void net_midi_control_change_subscribe(op_midi_t *s);
extern void net_midi_control_change_unsubscribe (op_midi_t *u);
extern void net_midi_program_change_subscribe(op_midi_t *s);
extern void net_midi_program_change_unsubscribe (op_midi_t *u);
extern void net_midi_real_time_subscribe(op_midi_t *s);
extern void net_midi_real_time_unsubscribe (op_midi_t *u);

typedef void(*midi_handler_t)(void* op, u32 event);


// add op pointer after the current position
//extern void net_midi_list_add(op_midi_t* op);
extern void net_midi_list_push(op_midi_t* op);
// delete op pointer at current position, return next pointer in list
extern void net_midi_list_remove(op_midi_t* op);
// handle incoming midi packet
extern void net_handle_midi_packet(u32 data);

EXTERN_C_END
#endif // h guard
