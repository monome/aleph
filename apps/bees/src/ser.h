#include "types.h"
#ifndef _SER_H_
#define _SER_H_
#include "op.h"
#include "op_math.h"
#include "types.h"
#include "ser.h"

void serial_process(s32 data);

//--- op_serial_t : comparisons
typedef struct op_serial_struct {
  op_t super;

  volatile io_t a;
  volatile io_t b;
  volatile io_t* in_val[2];

  op_out_t outs[];
} op_serial_t;


extern op_serial_t* last_serial_op;
void op_serial_out (op_serial_t* op, const io_t v);

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
#endif // header guard
