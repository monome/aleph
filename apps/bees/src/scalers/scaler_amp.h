/* scaler for amplitude (attentuation only)

 */

#include "scaler.h"
#include "types.h"

extern void scaler_init_amp(scaler* sc, const ParamDesc* desc);
extern s32 scaler_val_amp(scaler* sc, io_t in);
extern s32 scaler_rep_amp(scaler* sc, io_t in);
