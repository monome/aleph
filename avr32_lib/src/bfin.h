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
#include "types.h"

// blackfin HWAIT status
// extern volatile U8 hwait;

// 64k is max size of blackfin ldr file
//#define BFIN_LDR_MAX_BYTES 0x10000
//// actually, the ldr itself can be bigger than the bfin's sram...??
#define BFIN_LDR_MAX_BYTES 0x12000
//  32MB of SDRAM memory for aux buffers, envelopes and samples
#define BFIN_SAMPLES_MAX_BYTES 0x1e84800

// wait for busy pin to clear
void bfin_wait(void);

// load bfin from RAM buffer
//void bfin_load_buf(const u8* data, const u32 size);
void bfin_load_buf(volatile u8* data, u32 size);

// set a parameter
void bfin_set_param(U8 idx, fix16_t val);

//// UNTESTED:
fix16_t bfin_get_param(u8 idx);

// get number of parameters 
void bfin_get_num_params(volatile u32* num);
// get a parameter name
// void bfin_get_param_name(u16 paramIdx, volatile char* name);
// get parameter descriptor
void bfin_get_param_desc(u16 paramIdx, volatile ParamDesc* pDesc);
// get load module name
void bfin_get_module_name(volatile char* buf);
// get loaded module version
void bfin_get_module_version(ModuleVersion* vers);

// get recording head status
void get_headstatus(volatile u8 *status);

// clear and add params to ctl network
//void bfin_report_params(void);

// become slave on spi bus
//extern void bfin_spi_slave(void);

// become master on spi bus
extern void bfin_spi_master(void);

// enable audio processing
extern void bfin_enable(void);

// disable audio processing
extern void bfin_disable(void);

// get param value
extern s32 bfin_get_param(u8 idx);

// wait for ready status (e.g. after module init)
extern void bfin_wait_ready(void);

//PRGM
// set trig
extern void bfin_set_event(void);

//new sample
extern void bfin_new_sample(u32 offset, u32 size);

//transfer sample value
extern void bfin_sample(s32 val);

// set a sequenced parameter
extern void bfin_set_sqparam(u32 pos, u8 idx, fix16_t val);

//get buffer head state
extern u8 bfin_get_headstate(void);

<<<<<<< HEAD
//get play head position
extern u8 bfin_get_headposition(void);
=======
// fill a buffer on the blackfin with arbitrary data
extern void bfin_fill_buffer(const s32* src, u32 bytes);
>>>>>>> upstream/dev

#endif // header guard
