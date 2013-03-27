#ifndef _OP_PRESET_H_
#define _OP_PRESET_H_

#include "op_math.h"
#include "op.h"
#include "types.h"

//--- op_preset_t : preset store / recall
typedef struct op_preset_struct {
  op_t super;
  io_t idx;
} op_preset_t;
void op_preset_init(op_preset_t* preset);

#endif // header guard
