#include "midi_common.h"

#include "print_funcs.h"

void midi_packet_parse(midi_behavior_t *b, u32 data) {
	static u8 com;
	static u8 ch, num, val;
	static u16 bend;

	// print_dbg("\r\nmidi packet: 0x");
	// print_dbg_hex(data);

	// check status byte  
  com = (data & 0xf0000000) >> 28;
  ch  = (data & 0x0f000000) >> 24;

	// print_dbg("\r\nmidi packet ch: ");
	// print_dbg_ulong(ch);

	switch (com) {
  case 0x9:
		// note on
  	num = (data & 0xff0000) >> 16;
  	val = (data &   0xff00) >> 8;
		if (val == 0) {
			// note on with zero velocity is note off (per midi spec)
			if (b->note_off) b->note_off(ch, num, val);
		}
		else {
			if (b->note_on) b->note_on(ch, num, val);
		}
		break;
	case 0x8:
		// note off (with velocity)
    num = (data & 0xff0000) >> 16;
    val = (data &   0xff00) >> 8;
		if (b->note_off) b->note_off(ch, num, val);
		break;
	case 0xd:
		// channel pressure
		val = (data & 0x7f0000) >> 16;
		if (b->channel_pressure) b->channel_pressure(ch, val);
		break;
	case 0xe:
		// pitch bend
		bend = ((data & 0x00ff0000) >> 16) | ((data & 0xff00) >> 1);
		if (b->pitch_bend) b->pitch_bend(ch, bend);
		break;
	case 0xb:
		// control change
		num = (data & 0xff0000) >> 16;
    val = (data &   0xff00) >> 8;
		if (b->control_change) b->control_change(ch, num, val);
		break;
	case 0xf:
		// system message, ch is message type
		switch (ch) {
		case 0x8:
			if (b->clock_tick) b->clock_tick();
			break;
		case 0xa:
			if (b->seq_start) b->seq_start();
			break;
		case 0xb:
			if (b->seq_continue) b->seq_continue();
			break;
		case 0xc:
			if (b->seq_stop) b->seq_stop();
			break;
		}
		break;
		case 0xc:
			num = (data & 0x00ff0000) >> 16;
			if (b->program_change) b->program_change(ch, num);
			break;
	default:
		// TODO: poly pressure, program change, chanel mode *, rtc, etc
		break;
  }
}

void voice_flags_init(voice_flags_t *f) {
	f->sustain = 0;
	f->legato = 0;
	f->portamento = 0;
}

void voice_slot_init(voice_state_t *v, voice_alloc_mode mode, u8 count) {
	v->mode = mode;

	if (count < 1) {
		v->count = 1;
	}
	else if (count > MAX_VOICE_COUNT) {
		v->count = MAX_VOICE_COUNT;
	}
	else {
		v->count = count;
	}

	switch (mode) {
	case kVoiceAllocRotate:
	case kVoiceAllocLRU:
	default:
		// all modes have the same initial state at the moment
		v->head = 0;
		v->tail = 0;
		for (u8 i = 0; i < MAX_VOICE_COUNT; i++) {
			v->voice[i].active = 0;
			v->voice[i].num = 0;
		}
		break;
	}
}

u8 voice_slot_next(voice_state_t *v) {
	u8 h = 0;

	switch (v->mode) {
	case kVoiceAllocRotate:
		h = v->head;
		v->head++;
		if (!(v->head < v->count)) {
			v->head = 0;
		}
		break;
	case kVoiceAllocLRU:
		// TODO
		break;
	}

	return h;
}

u8 voice_slot_num(voice_state_t *v, u8 slot) {
	return v->voice[slot].num;
}

u8 voice_slot_active(voice_state_t *v, u8 slot) {
	return v->voice[slot].active;
}

void voice_slot_activate(voice_state_t *v, u8 slot, u8 num) {
	v->voice[slot].active = 1;
	v->voice[slot].num = num;
}

s8 voice_slot_find(voice_state_t *v, u8 num) {
	for (u8 i = 0; i < v->count; i++) {
		if (voice_slot_num(v, i) == num && voice_slot_active(v, i))
			return i;
	}
	return -1;
}

void voice_slot_release(voice_state_t *v, u8 slot) {
	switch (v->mode) {
	case kVoiceAllocRotate:
		v->voice[slot].active = 0;
		break;
	case kVoiceAllocLRU:
		// TODO
		break;
	}
}

void midi_clock_init(midi_clock_t *c) {
	c->running = true;
	c->pulse_period = 20;  // 20 ms is 120 bpm at 24 ppq
	c->pulse_count = 0;
	c->trigger = 0;
	midi_clock_set_div(c, 1);
}


void midi_clock_pulse(midi_clock_t *c, u16 period) {
	c->trigger = 0;
	if (period != 0) {
		c->pulse_period = period;
	}
	c->pulse_count++;
	if (c->pulse_count >= c->pulse_div_count) {
		c->pulse_count = 0;
		if (c->running) {
			c->trigger = 1;
		}
	}
}

bool midi_clock_set_div(midi_clock_t *c, u8 div) {
	if (div > 24 || div == 0) {
		return false;
	}
	c->pulse_div = div;
	c->pulse_div_count = 24 / div;
	c->pulse_count = 0;
	return true;
}

void midi_clock_start(midi_clock_t *c) {
	c->running = true;
	c->pulse_count = 0;
}

void midi_clock_stop(midi_clock_t *c) {
	c->running = false;
}

void midi_clock_continue(midi_clock_t *c) {
	c->running = true;
}
