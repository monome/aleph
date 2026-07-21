/*! meters.c

  Banked absolute peak meters. When MODULE_AUDIO_METER is 0, process is a
  no-op and snapshot returns zeros (SPI still handled).
*/

#include "meters.h"

#if MODULE_AUDIO_METER
#include "audio_channels.h"
#include "peak_meter_blk.h"
#endif

#if MODULE_AUDIO_METER

typedef struct {
  peak_meter_blk *ch;
  u8 n;
  u8 active;
} meter_bank_t;

static peak_meter_blk gMeterIn[METER_CH_PER_BANK];
static peak_meter_blk gMeterOut[METER_CH_PER_BANK];
static meter_bank_t gBanks[METER_BANK_MAX];

void meters_init(void) {
  u8 i;

  for(i = 0; i < METER_BANK_MAX; i++) {
    gBanks[i].ch = 0;
    gBanks[i].n = 0;
    gBanks[i].active = 0;
  }
  for(i = 0; i < METER_CH_PER_BANK; i++) {
    peak_meter_blk_init(&gMeterIn[i], MODULE_BLOCKSIZE);
    peak_meter_blk_init(&gMeterOut[i], MODULE_BLOCKSIZE);
  }
  gBanks[METER_BANK_IN].ch = gMeterIn;
  gBanks[METER_BANK_IN].n = METER_CH_PER_BANK;
  gBanks[METER_BANK_IN].active = 1;
  gBanks[METER_BANK_OUT].ch = gMeterOut;
  gBanks[METER_BANK_OUT].n = METER_CH_PER_BANK;
  gBanks[METER_BANK_OUT].active = 1;
}

u8 meters_register_bank(u8 bank_id, peak_meter_blk *ch, u8 n) {
  if(bank_id >= METER_BANK_MAX || n > METER_CH_PER_BANK || ch == 0) {
    return 0;
  }
  gBanks[bank_id].ch = ch;
  gBanks[bank_id].n = n;
  gBanks[bank_id].active = 1;
  return 1;
}

void meters_process(buffer_t *in, buffer_t *out) {
  u8 b;
  u8 c;
  u16 f;
  fract32 *p;
  meter_bank_t *bank;

  for(b = 0; b < METER_BANK_MAX; b++) {
    bank = &gBanks[b];
    if(!bank->active || bank->ch == 0) {
      continue;
    }
    for(c = 0; c < bank->n; c++) {
      peak_meter_blk_prepare(&bank->ch[c]);
    }
  }
  for(c = 0; c < METER_CH_PER_BANK; c++) {
    p = audio_in_channel(in, c);
    for(f = 0; f < MODULE_BLOCKSIZE; f++) {
      peak_meter_blk_next(&gMeterIn[c], p[f]);
    }
    p = audio_out_channel(out, c);
    for(f = 0; f < MODULE_BLOCKSIZE; f++) {
      peak_meter_blk_next(&gMeterOut[c], p[f]);
    }
  }
}

void meters_snapshot_bank(u8 bank, fract32 out[METER_CH_PER_BANK]) {
  u8 i;
  meter_bank_t *b;

  for(i = 0; i < METER_CH_PER_BANK; i++) {
    out[i] = 0;
  }
  if(bank >= METER_BANK_MAX) {
    return;
  }
  b = &gBanks[bank];
  if(!b->active || b->ch == 0) {
    return;
  }
  for(i = 0; i < b->n && i < METER_CH_PER_BANK; i++) {
    out[i] = peak_meter_blk_read(&b->ch[i]);
  }
}

#else /* !MODULE_AUDIO_METER */

void meters_init(void) {}

void meters_process(buffer_t *in, buffer_t *out) {
  (void)in;
  (void)out;
}

void meters_snapshot_bank(u8 bank, fract32 out[METER_CH_PER_BANK]) {
  u8 i;
  (void)bank;
  for(i = 0; i < METER_CH_PER_BANK; i++) {
    out[i] = 0;
  }
}

u8 meters_register_bank(u8 bank_id, peak_meter_blk *ch, u8 n) {
  (void)bank_id;
  (void)ch;
  (void)n;
  return 0;
}

#endif
