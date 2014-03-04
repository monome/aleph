/* op_adc.c

   ADC system operator, 4 channels.
   there should only ever be one of these, created at network initialzation
 */

// asf
#include "print_funcs.h"

// bees
#include "app_timers.h"
#include "net_protected.h"
#include "op_adc.h"

//-------------------------------------------------
//----- descriptor
static const char* op_adc_instring	= "ENABLE\0 PERIOD\0 MODE\0   ";
static const char* op_adc_outstring	= "VAL0\0   VAL1\0   VAL2\0   VAL3\0   ";
static const char* op_adc_opstring	= "CV-IN";

//-------------------------------------------------
//----- static function declaration
static void op_adc_in_enable	(op_adc_t* adc, const io_t v);
static void op_adc_in_period  (op_adc_t* adc, const io_t v);
static void op_adc_in_mode	(op_adc_t* adc, const io_t v);

// pickles
static u8* op_adc_pickle(op_adc_t* adc, u8* dst);
static const u8* op_adc_unpickle(op_adc_t* adc, const u8* src);

static op_in_fn op_adc_in_fn[3] = {
  (op_in_fn)&op_adc_in_enable,
  (op_in_fn)&op_adc_in_period,
  (op_in_fn)&op_adc_in_mode
};

// pickles
static u8* op_adc_pickle(op_adc_t* adc, u8* dst);
static const u8* op_adc_unpickle(op_adc_t* adc, const u8* src);

//---------------------------------------------
//----- external function definition

/// initialize
void op_adc_init(void* op) {
  op_adc_t* adc = (op_adc_t*)op;
  adc->super.numInputs = 3;
  adc->super.numOutputs = 4;

  adc->outs[0] = -1;
  adc->outs[1] = -1;
  adc->outs[2] = -1;
  adc->outs[3] = -1;

  adc->super.in_fn = op_adc_in_fn;
  // input value array
  adc->super.in_val = adc->in_val;
  adc->in_val[0] = &(adc->enable);
  adc->in_val[1] = &(adc->period);
  adc->in_val[2] = &(adc->mode);

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
  adc->val[0] = 0;
  adc->val[1] = 0;
  adc->val[2] = 0;
  adc->val[3] = 0;
  adc->period = op_from_int(50);
  adc->enable = 0;
  adc->mode = 0;

  adc->prev[0] = 0;
  adc->prev[1] = 0;
  adc->prev[2] = 0;
  adc->prev[3] = 0;
}

void op_adc_deinit(void* adc) {
  timers_unset_adc();
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input enable / disable
void op_adc_in_enable	(op_adc_t* adc, const io_t v) {
  print_dbg("\r\n op_adc_in_enable: 0x");
  print_dbg_hex((u32)(v));

  if((v) > 0) {
    print_dbg(" (input value high) ");
    if(adc->enable == 0) {
      adc->enable = OP_ONE;
      timers_set_adc(op_to_int(adc->period));
    }
  } else {
    print_dbg(" (input value low) ");
    if(adc->enable > 0) {
      adc->enable = 0;
      timers_unset_adc();
    }
  }
}

// input polling period
void op_adc_in_period (op_adc_t* adc, const io_t v) {
  if((v) < OP_ONE) {
    adc->period = OP_ONE;
  } else {
    adc->period = v;
  }
  timers_set_adc_period(op_to_int(adc->period));
}

void op_adc_in_mode (op_adc_t* adc, const io_t v) {
  if((v) < OP_ONE) {
    adc->mode = 0;
  } else {
    adc->mode = OP_ONE;
  }
}



//===== pickles

u8* op_adc_pickle(op_adc_t* adc, u8* dst) {
  dst = pickle_io(adc->enable, dst);
  dst = pickle_io(adc->period, dst);
  dst = pickle_io(adc->mode, dst);
  return dst;
}

const u8* op_adc_unpickle(op_adc_t* adc, const u8* src) {
  src = unpickle_io(src, &(adc->enable));
  src = unpickle_io(src, &(adc->period));
  src = unpickle_io(src, &(adc->mode));
  if(adc->enable) {
    timers_set_adc(op_to_int(adc->period));    
  }
  return src;
}


void op_adc_sys_input(op_adc_t* adc, u8 ch, u16 val) {
  if(adc->mode) {
    u8 i = val > 1000;    // thresh around 2.5v ?
    if(adc->prev[ch] != i) {
      net_activate(adc->outs[ch], i, &(adc->super));
      adc->prev[ch] = i;
    }
  }
  else {
    adc->val[ch] = val;

    net_activate(adc->outs[ch], val, &(adc->super));
  }
}
