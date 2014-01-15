/* scaler for 1-pole integrator, decay time

 */

#ifndef _ALEPH_BEES_SCALER_INTEGRATOR_H_
#define _ALEPH_BEES_SCALER_INTEGRATOR_H_

#include "param_scaler.h"
#include "types.h"

//#define PARAM_SCALER_INTEGRATOR_DATA_SIZE (1024 * 4 * 2)
#define PARAM_SCALER_INTEGRATOR_DATA_SIZE (1024 * 4)

extern void scaler_integrator_init(void* scaler);
extern s32 scaler_integrator_val(void* scaler, io_t in);
extern void scaler_integrator_str(char* dst, void* scaler, io_t in);
extern io_t scaler_integrator_in(void* scaler, s32 val);
extern s32 scaler_integrator_inc(void* sc, io_t *pin, io_t inc );

#endif
