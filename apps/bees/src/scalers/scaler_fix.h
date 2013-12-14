/* scaler for 16.16 linear

 */

#ifndef _ALEPH_BEES_SCALER_FIX_H_
#define _ALEPH_BEES_SCALER_FIX_H_

#include "param_scaler.h"
#include "types.h"

#define PARAM_SCALER_FIX_DATA_SIZE 0

extern void scaler_fix_init(void* scaler);
extern s32 scaler_fix_val(void* scaler, io_t in);
extern void scaler_fix_str(char* dst, void* scaler, io_t in);
extern io_t scaler_fix_in(void* scaler, s32 val);
extern s32 scaler_fix_inc(void* sc, io_t *pin, io_t inc );

#endif
