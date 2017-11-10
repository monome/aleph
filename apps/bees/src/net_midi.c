// asf
#include "print_funcs.h"

// bees
#include "net_midi.h"


//--------------
//--- types

//-----------------------------
//-- static variables

void net_midi_init(op_midi_t *midi) {
  // we need to zero all the memory, mostly to ensure that unassigned
  // callbacks are NULL
  int i;
  u8 *m = (u8*) midi;
  for(i=0; i < sizeof(m); i++) {
    m[i] = 0;
  }
}

static void net_midi_note_on (u8 ch, u8 num, u8 vel);
static void net_midi_note_off (u8 ch, u8 num, u8 vel);
static void net_midi_channel_pressure (u8 ch, u8 val);
static void net_midi_pitch_bend (u8 ch, u16 bend);
static void net_midi_control_change (u8 ch, u8 num, u8 val);
static void net_midi_program_change (u8 ch, u8 num);
static void net_midi_clock_tick (void);
static void net_midi_seq_start (void);
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
  .seq_start = net_midi_seq_start,
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
    if(m->handler.note_on) {
      m->handler.note_on(m->sub, ch, num, vel);
    }
    m = m->next;
  }
}

void net_midi_note_off (u8 ch, u8 num, u8 vel) {
  op_midi_t *m = midi_note_subscribers;
  while(m != NULL) {
    if(m->handler.note_off) {
      m->handler.note_off(m->sub, ch, num, vel);
    }
    m = m->next;
  }
}

void net_midi_channel_pressure (u8 ch, u8 val) {
  op_midi_t *m = midi_note_subscribers;
  while(m != NULL) {
    if(m->handler.channel_pressure) {
      m->handler.channel_pressure(m->sub, ch, val);
    }
    m = m->next;
  }
}

void net_midi_pitch_bend (u8 ch, u16 bend) {
  op_midi_t *m = midi_note_subscribers;
  while(m != NULL) {
    if(m->handler.pitch_bend) {
      m->handler.pitch_bend(m->sub, ch, bend);
    }
    m = m->next;
  }
}

void net_midi_control_change (u8 ch, u8 num, u8 val) {
  op_midi_t *m = midi_control_change_subscribers;
  while(m != NULL) {
    if(m->handler.control_change) {
      m->handler.control_change(m->sub, ch, num, val);
    }
    m = m->next;
  }
}

void net_midi_program_change (u8 ch, u8 num) {
  op_midi_t *m = midi_program_change_subscribers;
  while(m != NULL) {
    if(m->handler.program_change) {
      m->handler.program_change(m->sub, ch, num);
    }
    m = m->next;
  }
}

void net_midi_clock_tick (void) {
  op_midi_t *m = midi_real_time_subscribers;
  while(m != NULL) {
    if(m->handler.clock_tick) {
      m->handler.clock_tick(m->sub);
    }
    m = m->next;
  }
}

void net_midi_seq_stop (void) {
  op_midi_t *m = midi_real_time_subscribers;
  while(m != NULL) {
    if(m->handler.seq_stop) {
      m->handler.seq_stop(m->sub);
    }
    m = m->next;
  }
}

void net_midi_seq_start (void) {
  op_midi_t *m = midi_real_time_subscribers;
  while(m != NULL) {
    if(m->handler.seq_start) {
      m->handler.seq_start(m->sub);
    }
    m = m->next;
  }
}

void net_midi_seq_continue (void) {
  op_midi_t *m = midi_real_time_subscribers;
  while(m != NULL) {
    if(m->handler.seq_continue) {
      m->handler.seq_continue(m->sub);
    }
    m = m->next;
  }
}

void net_midi_panic (void) {
  // all subscribed lists should get the panic signal
  op_midi_t *m[4] = { midi_note_subscribers,
		      midi_control_change_subscribers,
		      midi_program_change_subscribers,
		      midi_real_time_subscribers};
  int i;
  for(i=0; i < 4; i++) {
    while(m[i] != NULL) {
      if(m[i]->handler.panic) {
	m[i]->handler.panic(m[i]->sub);
      }
      m[i] = m[i]->next;
    }
  }
}
