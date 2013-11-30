/* scaler for 1.32 linear

 */

#ifndef _ALEPH_BEES_SCALER_FRACT_H_
#define _ALEPH_BEES_SCALER_FRACT_H_

#include "param_scaler.h"
#include "types.h"

extern void scaler_fract_init(ParamScaler* sc, const ParamDesc* desc);
extern s32 scaler_fract_val(io_t in);
extern void scaler_fract_str(char* dst, io_t in);
extern io_t scaler_fract_in(s32 val);

#endif
