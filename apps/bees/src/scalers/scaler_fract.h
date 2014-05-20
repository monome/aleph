/* scaler for 1.31 fract

 */

#ifndef _ALEPH_BEES_SCALER_FRACT_H_
#define _ALEPH_BEES_SCALER_FRACT_H_

#include "param_scaler.h"
#include "types.h"

#define PARAM_SCALER_FRACT_DATA_SIZE 0

extern void scaler_fract_init(void* scaler);
extern s32 scaler_fract_val(void* scaler, io_t in);
extern void scaler_fract_str(char* dst, void* scaler, io_t in);
extern io_t scaler_fract_in(void* scaler, s32 val);
extern s32 scaler_fract_inc(void* sc, io_t *pin, io_t inc );

#endif
