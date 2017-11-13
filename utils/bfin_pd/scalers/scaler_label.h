/* scaler for patch matrix

 */

#ifndef _ALEPH_BEES_SCALER_PATCH_H_
#define _ALEPH_BEES_SCALER_PATCH_H_


#include "param_scaler.h"
#include "types.h"


#define PARAM_SCALER_PATCH_DATA_SIZE (1024 * 4 * 2)

extern void scaler_start_parse_labels (void);
extern void scaler_parse_labels_char (char c);
extern void scaler_labels_init(void* scaler);
extern s32 scaler_labels_val(void* scaler, io_t in);
extern void scaler_labels_str(char* dst, void* scaler, io_t in);
extern io_t scaler_labels_in(void* scaler, s32 val);
extern s32 scaler_labels_inc(void* sc, io_t *pin, io_t inc );

#define MAX_LABELS 64
#define LABEL_LENGTH 64

extern char scaler_labels[MAX_LABELS][LABEL_LENGTH];
extern int scaler_n_labels;


#endif
