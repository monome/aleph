#ifndef _OP_SERIAL_H_
#define _OP_SERIAL_H_

#include "op.h"
#include "op_math.h"
#include "types.h"
#include "ser.h"

extern void op_serial_init(void* op);
extern void op_serial_deinit(void* op);

void op_serial_out (op_serial_t* op, const io_t v);

#endif // header guard
