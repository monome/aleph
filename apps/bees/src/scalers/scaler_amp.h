/* scaler for amplitude (attentuation only)

 */

#ifndef _ALEPH_BEES_SCALER_AMP_H_
#define _ALEPH_BEES_SCALER_AMP_H_

#include "param_scaler.h"
#include "types.h"

extern void scaler_amp_init(ParamScaler* sc, const ParamDesc* desc);
extern s32 scaler_amp_val(io_t in);
extern void scaler_amp_str(char* dst, io_t in);
extern io_t scaler_amp_in(s32 val);

#endif
