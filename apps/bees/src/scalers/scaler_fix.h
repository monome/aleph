/* scaler for 16.16 linear

 */

#ifndef _ALEPH_BEES_SCALER_FIX_H_
#define _ALEPH_BEES_SCALER_FIX_H_

#include "param_scaler.h"
#include "types.h"

extern void scaler_fix_init(ParamScaler* sc, const ParamDesc* desc);
extern s32 scaler_fix_val(io_t in);
extern void scaler_fix_str(char* dst, io_t in);
extern io_t scaler_fix_in(s32 val);

#endif
