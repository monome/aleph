
#ifndef _NULLP_MODULE_INTERFACE_H_
#define _NULLP_MODULE_INTERFACE_H_

#include "types.h"

//=====================
// ==== these are for the 'nullp' emulator

extern void module_init(const u32 samplerate);
extern void module_callback(const f32* in, f32* out);
extern void module_set_param(u32 idx, f32 val);
extern void module_deinit(void);

#endif // header guard
