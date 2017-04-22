/* scaler for patch matrix

 */

#ifndef _ALEPH_BEES_SCALER_PATCH_H_
#define _ALEPH_BEES_SCALER_PATCH_H_


#include "param_scaler.h"
#include "types.h"
EXTERN_C_BEGIN

#define PARAM_SCALER_PATCH_DATA_SIZE (1024 * 4 * 2)

extern void scaler_start_parse_patchnames (void);
extern void scaler_parse_patchname_char (char c);
extern void scaler_patch_init(void* scaler);
extern s32 scaler_patch_val(void* scaler, io_t in);
extern void scaler_patch_str(char* dst, void* scaler, io_t in);
extern io_t scaler_patch_in(void* scaler, s32 val);
extern s32 scaler_patch_inc(void* sc, io_t *pin, io_t inc );

#define MAX_PATCHPOINTS 64
#define PATCHPOINT_LENGTH 64

extern char scaler_patch_patchPoints[MAX_PATCHPOINTS][PATCHPOINT_LENGTH];
extern int scaler_n_patchPoints;

EXTERN_C_END
#endif
