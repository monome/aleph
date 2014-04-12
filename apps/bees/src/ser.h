#include "types.h"

void serial_process(s32 data);

// typedef void(*process_serial_t)(u16);

// extern void serial_param_num(s32 data);
// extern void serial_param_info(s32 data);
// extern void serial_param_get(s32 data);
// extern void serial_param_set(s32 data);


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
