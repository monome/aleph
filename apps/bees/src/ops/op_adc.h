#ifndef _ALEPH_BEES_OP_ADC_H_
#define _ALEPH_BEES_OP_ADC_H_

#include "op.h"
#include "op_math.h"
#include "types.h"

//--- op_adc_t : adc source

// this really doesn't do much but it is a distinct operator type for usability.

typedef struct op_adc_struct {
  op_t super;
  // in:
  // enable, update period, 4 values
  volatile io_t* in_val[6];
  volatile io_t enable;
  volatile io_t period;
  volatile io_t val0;
  volatile io_t val1;
  volatile io_t val2;
  volatile io_t val3;
  // out: 4 values
  op_out_t outs[4];
} op_adc_t;

void op_adc_init(void* op);
void op_adc_deinit(void* op);

#endif // header guard
