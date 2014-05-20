/* scaler for 16b signed integer

 */

#ifndef _ALEPH_BEES_SCALER_SHORT_H_
#define _ALEPH_BEES_SCALER_SHORT_H_

#include "param_scaler.h"
#include "types.h"

#define PARAM_SCALER_SHORT_DATA_SIZE 0

extern void scaler_short_init(void* scaler);
extern s32 scaler_short_val(void* scaler, io_t in);
extern void scaler_short_str(char* dst, void* scaler, io_t in);
extern io_t scaler_short_in(void* scaler, s32 val);
extern s32 scaler_short_inc(void* sc, io_t *pin, io_t inc );

#endif
