/* scaler for 1-pole 16b integrator, decay time

 */
#ifndef _ALEPH_BEES_SCALER_INTEGRATOR_SHORT_H_
#define _ALEPH_BEES_SCALER_INTEGRATOR_SHORT_H_

#include "param_scaler.h"
#include "types.h"

//#define PARAM_SCALER_INTEGRATOR_SHORT_DATA_SIZE (1024 * 4 * 2)
#define PARAM_SCALER_INTEGRATOR_SHORT_DATA_SIZE (1024 * 4)

extern void scaler_integrator_short_init(void* scaler);
extern s32 scaler_integrator_short_val(void* scaler, io_t in);
extern void scaler_integrator_short_str(char* dst, void* scaler, io_t in);
extern io_t scaler_integrator_short_in(void* scaler, s32 val);
extern s32 scaler_integrator_short_inc(void* sc, io_t *pin, io_t inc );

#endif // h guard
