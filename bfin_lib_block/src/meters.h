/*! meters.h

  Banked peak meters for SPI readout (MSG_GET_METER_COM).
  I/O metering is opt-in via MODULE_AUDIO_METER in module_custom.h.
*/

#ifndef _BFIN_METERS_H_
#define _BFIN_METERS_H_

#include "audio.h"
#include "peak_meter_blk.h"
#include "types.h"

#ifndef MODULE_AUDIO_METER
#define MODULE_AUDIO_METER 0
#endif

#define METER_BANK_IN 0
#define METER_BANK_OUT 1
#define METER_BANK_MAX 8
#define METER_CH_PER_BANK 4

void meters_init(void);
void meters_process(buffer_t *in, buffer_t *out);
void meters_snapshot_bank(u8 bank, fract32 out[METER_CH_PER_BANK]);
/* Register peak_meter_blk array for a bank (n <= 4; unused ch read as 0).
 * Returns 1 on success, 0 if bank_id invalid or n too large. */
u8 meters_register_bank(u8 bank_id, peak_meter_blk *ch, u8 n);

#endif
