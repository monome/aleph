/* scaler for boolean

 */

#ifndef _ALEPH_BEES_SCALER_BOOL_H_
#define _ALEPH_BEES_SCALER_BOOL_H_

#include "param_scaler.h"
#include "types.h"

#define PARAM_SCALER_BOOL_DATA_SIZE 0

extern void scaler_bool_init(void* scaler);
extern s32 scaler_bool_val(void* scaler, io_t in);
extern void scaler_bool_str(char* dst, void* scaler, io_t in);
extern io_t scaler_bool_in(void* scaler, s32 val);
extern s32 scaler_bool_inc(void* sc, io_t *pin, io_t inc );

#endif
