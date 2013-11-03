/* op_adc.c

   ADC system operator, 4 channels.
   there should only ever be one of these, created at network initialzation
 */

#include "app_timers.h"
#include "net_protected.h"
#include "op_adc.h"

//-------------------------------------------------
//----- descriptor
static const char* op_adc_instring	= "ENABLE  PERIOD  VAL0    VAL1    VAL2    VAL3    ";
static const char* op_adc_outstring	= "VAL0    VAL1    VAL2    VAL3    ";
static const char* op_adc_opstring	= "ADC";

//-------------------------------------------------
//----- static function declaration
static void op_adc_inc_fn	(op_adc_t* adc, const s16 idx, const io_t inc);
static void op_adc_in_enable	(op_adc_t* adc, const io_t* v);
static void op_adc_in_period	(op_adc_t* adc, const io_t* v);
static void op_adc_in_val0	(op_adc_t* adc, const io_t* v);
static void op_adc_in_val1	(op_adc_t* adc, const io_t* v);
static void op_adc_in_val2	(op_adc_t* adc, const io_t* v);
static void op_adc_in_val3	(op_adc_t* adc, const io_t* v);

// pickles
static u8* op_adc_pickle(op_adc_t* adc, u8* dst);
static const u8* op_adc_unpickle(op_adc_t* adc, const u8* src);

static op_in_fn op_adc_in_fn[6] = {
  (op_in_fn)&op_adc_in_enable,
  (op_in_fn)&op_adc_in_period,
  (op_in_fn)&op_adc_in_val0,
  (op_in_fn)&op_adc_in_val1,
  (op_in_fn)&op_adc_in_val2,
  (op_in_fn)&op_adc_in_val3,
};

// pickles
static u8* op_adc_pickle(op_adc_t* adc, u8* dst);
static const u8* op_adc_unpickle(op_adc_t* adc, const u8* src);

//---------------------------------------------
//----- external function definition

/// initialize
void op_adc_init(void* op) {
  op_adc_t* adc = (op_adc_t*)op;
  adc->super.numInputs = 4;
  adc->super.numOutputs = 6;
  adc->outs[0] = -1;
  adc->outs[1] = -1;
  adc->outs[2] = -1;
  adc->outs[3] = -1;
  // ui increment function
  adc->super.inc_fn = (op_inc_fn)op_adc_inc_fn;
  adc->super.in_fn = op_adc_in_fn;
  // input function array
  adc->super.in_val = adc->in_val;
  // pickles
  adc->super.pickle = (op_pickle_fn)(&op_adc_pickle);
  adc->super.unpickle = (op_unpickle_fn)(&op_adc_unpickle);
  // output array
  adc->super.out = adc->outs;
  // strings
  adc->super.opString = op_adc_opstring;
  adc->super.inString = op_adc_instring;
  adc->super.outString = op_adc_outstring;
  // type
  adc->super.type = eOpAdc;
  adc->super.flags |= (1 << eOpFlagSys);
  /// state
  adc->val0 = 0;
  adc->val1 = 0;
  adc->val2 = 0;
  adc->val3 = 0;
  adc->period = 10;
  adc->enable = 0;
}

void op_adc_deinit(void* adc) {
  timers_unset_adc();
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input enable / disable
void op_adc_in_enable	(op_adc_t* adc, const io_t* v) {
  if(*v > 0) {
    adc->enable = OP_ONE;
    timers_set_adc(OP_TO_INT(adc->period));
  } else {
    adc->enable = 0;
    timers_unset_adc();
  }
}

// input polling period
void op_adc_in_period (op_adc_t* adc, const io_t* v) {
  if(*v < OP_ONE) {
    adc->period = OP_ONE;
  } else {
    adc->period = *v;
  }
  timers_set_adc_period(OP_TO_INT(adc->period));
}


// input value
static void op_adc_in_val0(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val0 = *v;
  net_activate(adc->outs[0], adc->val0, &(adc->super));
}

// input value
static void op_adc_in_val1(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val1 = *v;
  net_activate(adc->outs[1], adc->val1, &(adc->super));
}

// input value
static void op_adc_in_val2(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val2 = *v;
  net_activate(adc->outs[2], adc->val2, &(adc->super));
}

// input value
static void op_adc_in_val3(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val3 = *v;
  net_activate(adc->outs[3], adc->val3, &(adc->super));
}

// ===== UI input

// increment
static void op_adc_inc_fn(op_adc_t* adc, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0: // enable (toggle)
    if(adc->enable) {
      val = 0;
      op_adc_in_enable(adc, &val);
    } else {
      val = OP_ONE;
      op_adc_in_enable(adc, &val);
    }
    break;
  case 1: // period
    val = OP_SADD(adc->period, inc);
    op_adc_in_period(adc, &val);
    break;
  case 2: // val0
    val = OP_SADD(adc->val0, inc);
    op_adc_in_val0(adc, &val);
    break;
  case 3: // val1
    val = OP_SADD(adc->val1, inc);
    op_adc_in_val1(adc, &val);
    break;
  case 4: // val2
    val = OP_SADD(adc->val2, inc);
    op_adc_in_val2(adc, &val);
    break;
  case 5: // val3
    val = OP_SADD(adc->val3, inc);
    op_adc_in_val3(adc, &val);
    break;
  }
}

//===== pickles

u8* op_adc_pickle(op_adc_t* adc, u8* dst) {
  // no state
  return dst;
}

const u8* op_adc_unpickle(op_adc_t* adc, const u8* src) {
  // no state 
  return src;
}
