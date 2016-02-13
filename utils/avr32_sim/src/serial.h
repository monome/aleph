#ifndef _ALEPH_SERIAL_H_
#define _ALEPH_SERIAL_H_

#include "types.h"

#define SERIAL_BUFFER_SIZE		1024

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*process_serial_t)(u16);

extern volatile u8 serial_buffer[SERIAL_BUFFER_SIZE];

extern void serial_process(void);

extern void serial_send_start(u8);
extern void serial_send_byte(u8);
extern void serial_send_long(u32);
extern void serial_send_separator(void);
extern void serial_send_end(void);

extern void serial_param_num(s32 data);
extern void serial_param_info(s32 data);
extern void serial_param_get(s32 data);
extern void serial_param_set(s32 data);


#ifdef __cplusplus
}
#endif

#endif // h guard
