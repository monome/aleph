/* scaler for amplitude (attentuation only)

 */

#include "scaler.h"
#include "types.h"

extern void scaler_init_amp(fix16 min, fix16 max);
extern s32 scaler_val_amp(io_t in);
extern fix16 scaler_rep_amp(io_t in);
