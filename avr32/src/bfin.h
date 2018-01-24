/* bfin.h
   aleph-avr32

   routines to communicate with the bf533 DSP
*/

#ifndef _BFIN_H_
#define _BFIN_H_

#include "compiler.h"
#include "fix.h"
#include "module_common.h"
#include "param_common.h"

//! max size of blackfin ldr file
#define BFIN_LDR_MAX_BYTES 0x12000

//! wait for busy pin to clear
void bfin_wait(void);

//! load bfin from RAM buffer
void bfin_load_buf(const u8* data, const u32 size);

//! set a parameter value
void bfin_set_param(U8 idx, fix16_t val);

//! get a parameter value
fix16_t bfin_get_param(u8 idx);

//! get number of parameters
void bfin_get_num_params(volatile u32* num);

//! get load module name
void bfin_get_module_name(volatile char* buf);

//! get loaded module version
void bfin_get_module_version(ModuleVersion* vers);

//! enable audio processing
extern void bfin_enable(void);

//! disable audio processing
extern void bfin_disable(void);

//! wait for ready status (e.g. after module init)
extern void bfin_wait_ready(void);

//! get param value
extern s32 bfin_get_param(u8 idx);

//! get audio change cpu usage
extern s32 bfin_get_audio_cpu(void);

//! get param change cpu usage
extern s32 bfin_get_control_cpu(void);

//! fill a buffer on the blackfin with arbitrary data
extern void bfin_fill_buffer(const s32* src, u32 bytes);

#endif  // header guard
