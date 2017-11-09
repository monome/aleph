// asf
#include "print_funcs.h"

// bees
#include "net_midi.h"


//--------------
//--- types

//-----------------------------
//-- static variables

static void net_midi_note_on (u8 ch, u8 num, u8 vel);
static void net_midi_note_off (u8 ch, u8 num, u8 vel);
static void net_midi_channel_pressure (u8 ch, u8 val);
static void net_midi_pitch_bend (u8 ch, u16 bend);
static void net_midi_control_change (u8 ch, u8 num, u8 val);
static void net_midi_program_change (u8 ch, u8 num);
static void net_midi_clock_tick (void);
static void net_midi_seq_stop (void);
static void net_midi_seq_continue (void);
static void net_midi_panic (void);

static midi_behavior_t net_midi_behaviour = {
  .note_on = net_midi_note_on,
  .note_off = net_midi_note_off,
  .channel_pressure = net_midi_channel_pressure,
  .pitch_bend = net_midi_pitch_bend,
  .control_change = net_midi_control_change,
  .program_change = net_midi_program_change,
  .clock_tick = net_midi_clock_tick,
  .seq_stop = net_midi_seq_stop,
  .seq_continue = net_midi_seq_continue,
  .panic = net_midi_panic
};

void net_handle_midi_packet(u32 data) {
  midi_packet_parse(&net_midi_behaviour, data);
}

static void net_midi_subscribe (op_midi_t *newSubscriber, op_midi_t **list) {
  newSubscriber->next = *list;
  *list = newSubscriber;
}
static void net_midi_unsubscribe (op_midi_t *u, op_midi_t **list) {
  if(u == *list) {
    *list = (*list)->next;
    return;
  }
  op_midi_t *m = *list;
  while(m != NULL) {
    if(m->next == u) {
      m->next = u->next;
    }
    m = m->next;
  }
}

static op_midi_t *midi_note_subscribers = NULL;
static op_midi_t *midi_control_change_subscribers = NULL;
static op_midi_t *midi_program_change_subscribers = NULL;
static op_midi_t *midi_real_time_subscribers = NULL;

void net_midi_note_subscribe(op_midi_t *s) {
  net_midi_subscribe(s, &midi_note_subscribers);
}
void net_midi_note_unsubscribe (op_midi_t *u) {
  net_midi_unsubscribe(u, &midi_note_subscribers);
}
void net_midi_control_change_subscribe(op_midi_t *s) {
  net_midi_subscribe(s, &midi_control_change_subscribers);
}
void net_midi_control_change_unsubscribe (op_midi_t *u) {
  net_midi_unsubscribe(u, &midi_control_change_subscribers);
}
void net_midi_program_change_subscribe(op_midi_t *s) {
  net_midi_subscribe(s, &midi_program_change_subscribers);
}
void net_midi_program_change_unsubscribe (op_midi_t *u) {
  net_midi_unsubscribe(u, &midi_program_change_subscribers);
}
void net_midi_real_time_subscribe(op_midi_t *s) {
  net_midi_subscribe(s, &midi_real_time_subscribers);
}
void net_midi_real_time_unsubscribe (op_midi_t *u) {
  net_midi_unsubscribe(u, &midi_real_time_subscribers);
}

void net_midi_note_on (u8 ch, u8 num, u8 vel) {
  op_midi_t *m = midi_note_subscribers;
  while(m != NULL) {
    midi_note_on_t h = (midi_note_on_t) m->handler;
    h(ch, num, vel);
    m = m->next;
  }
}

void net_midi_note_off (u8 ch, u8 num, u8 vel) {
  op_midi_t *m = midi_note_subscribers;
  while(m != NULL) {
    midi_note_off_t h = (midi_note_off_t) m->handler;
    h(ch, num, vel);
    m = m->next;
  }
}

void net_midi_channel_pressure (u8 ch, u8 val) {
}
void net_midi_pitch_bend (u8 ch, u16 bend) {
}
void net_midi_control_change (u8 ch, u8 num, u8 val) {
}
void net_midi_program_change (u8 ch, u8 num) {
}
void net_midi_clock_tick (void) {
}
void net_midi_seq_stop (void) {
}
void net_midi_seq_continue (void) {
}
void net_midi_panic (void) {
}
