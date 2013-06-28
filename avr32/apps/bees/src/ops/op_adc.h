#ifndef _ALEPH_BEES_OP_ADC_H_
#define _ALEPH_BEES_OP_ADC_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_adc_t : adc source

// this really doesn't do much but it is a distinct operator type for usability.

typedef struct op_adc_struct {
  op_t super;
  io_t* in_val[1];
  io_t* val;
  op_out_t outs[1];
} op_adc_t;
void op_adc_init(op_adc_t* adc);


#endif // header guard
