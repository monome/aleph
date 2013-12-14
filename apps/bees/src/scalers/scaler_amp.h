/* scaler for amplitude (attentuation only)

 */

#ifndef _ALEPH_BEES_SCALER_AMP_H_
#define _ALEPH_BEES_SCALER_AMP_H_

#include "param_scaler.h"
#include "types.h"

#define PARAM_SCALER_AMP_DATA_SIZE (1024 * 4 * 2)

extern void scaler_amp_init(void* scaler);
extern s32 scaler_amp_val(void* scaler, io_t in);
extern void scaler_amp_str(char* dst, void* scaler, io_t in);
extern io_t scaler_amp_in(void* scaler, s32 val);
extern s32 scaler_amp_inc(void* sc, io_t *pin, io_t inc );

#endif
