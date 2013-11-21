#include "types.h"

#define SERIAL_BUFFER_SIZE		1024

typedef void(*process_serial_t)(u16);

extern volatile u8 serial_buffer[SERIAL_BUFFER_SIZE];

extern void serial_process(void);
extern void serial_send_start(u8);
extern void serial_send_byte(u8);
extern void serial_send_long(u32);
extern void serial_send_separator(void);
extern void serial_send_end(void);


// incoming serial:

// debug(int state)
// req_num_params(void);
// req_param_info(u8 index)
// set_param_val(u8 index, u32 val)
// get param_val(u8 index)

// outgoing serial:
// debug
// report_num_params(u8 num)
// report_param_info(u8 index, "name")
// report_param_val(u8 index, u32 val)
