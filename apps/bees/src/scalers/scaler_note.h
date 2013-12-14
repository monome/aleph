/* scaler for midi note + 8-step micro

 */

#ifndef _ALEPH_BEES_SCALER_NOTE_H_
#define _ALEPH_BEES_SCALER_NOTE_H_

#include "param_scaler.h"
#include "types.h"

#define PARAM_SCALER_NOTE_DATA_SIZE (1024 * 4)

extern void scaler_note_init(void* scaler);
extern s32 scaler_note_val(void* scaler, io_t in);
extern void scaler_note_str(char* dst, void* scaler, io_t in);
extern io_t scaler_note_in(void* scaler, s32 val);
extern s32 scaler_note_inc(void* sc, io_t *pin, io_t inc );

#endif
