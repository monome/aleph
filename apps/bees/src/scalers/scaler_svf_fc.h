/* scaler for svf corner frequency (as midi note)

 */

#ifndef _ALEPH_BEES_SCALER_SVF_FC_H_
#define _ALEPH_BEES_SCALER_SVF_FC_H_

#include "param_scaler.h"
#include "types.h"

#define PARAM_SCALER_SVF_FC_DATA_SIZE (1024 * 4)

extern void scaler_svf_fc_init(void* scaler);
extern s32 scaler_svf_fc_val(void* scaler, io_t in);
extern void scaler_svf_fc_str(char* dst, void* scaler, io_t in);
extern io_t scaler_svf_fc_in(void* scaler, s32 val);
extern s32 scaler_svf_fc_inc(void* sc, io_t *pin, io_t inc );

#endif
