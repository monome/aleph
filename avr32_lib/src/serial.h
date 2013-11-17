#include "types.h"

#define SERIAL_BUFFER_SIZE		128
// #define SERIAL_NUM_COMMANDS		4

typedef void(*process_serial_t)(void);

extern u8 serial_buffer[SERIAL_BUFFER_SIZE];
extern u8 serial_read_pos;
extern u8 serial_write_pos;
extern u8 serial_new_bytes;

extern void serial_process(void);

void com_req_num_params(void);
void com_req_param_info(void);
void com_get_param(void);
void com_set_param(void);

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