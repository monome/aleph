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
static const char* op_adc_instring	= "ENABLE  PERIOD";
static const char* op_adc_outstring	= "VAL0    VAL1    VAL2    VAL3    ";
static const char* op_adc_opstring	= "ADC";

//-------------------------------------------------
//----- static function declaration
static void op_adc_inc_fn	(op_adc_t* adc, const s16 idx, const io_t inc);
static void op_adc_in_enable	(op_adc_t* adc, const io_t v);
static void op_adc_in_period	(op_adc_t* adc, const io_t v);
/* static void op_adc_in_val0	(op_adc_t* adc, const io_t v); */
/* static void op_adc_in_val1	(op_adc_t* adc, const io_t v); */
/* static void op_adc_in_val2	(op_adc_t* adc, const io_t v); */
/* static void op_adc_in_val3	(op_adc_t* adc, const io_t v); */

// pickles
static u8* op_adc_pickle(op_adc_t* adc, u8* dst);
static const u8* op_adc_unpickle(op_adc_t* adc, const u8* src);

static op_in_fn op_adc_in_fn[2] = {
  (op_in_fn)&op_adc_in_enable,
  (op_in_fn)&op_adc_in_period,
  /* (op_in_fn)&op_adc_in_val0, */
  /* (op_in_fn)&op_adc_in_val1, */
  /* (op_in_fn)&op_adc_in_val2, */
  /* (op_in_fn)&op_adc_in_val3, */
};

// pickles
static u8* op_adc_pickle(op_adc_t* adc, u8* dst);
static const u8* op_adc_unpickle(op_adc_t* adc, const u8* src);

//---------------------------------------------
//----- external function definition

/// initialize
void op_adc_init(void* op) {
  op_adc_t* adc = (op_adc_t*)op;
  adc->super.numInputs = 2;
  adc->super.numOutputs = 4;

  adc->outs[0] = -1;
  adc->outs[1] = -1;
  adc->outs[2] = -1;
  adc->outs[3] = -1;
  // ui increment function
  adc->super.inc_fn = (op_inc_fn)op_adc_inc_fn;
  adc->super.in_fn = op_adc_in_fn;
  // input value array
  adc->super.in_val = adc->in_val;
  adc->in_val[0] = &(adc->enable);
  adc->in_val[1] = &(adc->period);

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
  adc->period = op_from_int(20);
  adc->enable = 0;
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


/* // input value */
/* static void op_adc_in_val0(op_adc_t* adc, const io_t v) { */
/*   // simply passes value to output */
/*   adc->val0 = v; */
/*   //  print_dbg("\r\n adc op output, channel 0, value: 0x"); */
/*   //  print_dbg_hex(adc->val0); */
/*   net_activate(adc->outs[0], adc->val0, &(adc->super)); */
/* } */

/* // input value */
/* static void op_adc_in_val1(op_adc_t* adc, const io_t v) { */
/*   // simply passes value to output */
/*   adc->val1 = v; */
/*   //  print_dbg("\r\n adc op output, channel 1, value: 0x"); */
/*   //  print_dbg_hex(adc->val1); */
/*   net_activate(adc->outs[1], adc->val1, &(adc->super)); */
/* } */

/* // input value */
/* static void op_adc_in_val2(op_adc_t* adc, const io_t v) { */
/*   // simply passes value to output */
/*   adc->val2 = v; */
/*   //  print_dbg("\r\n adc op output, channel 2, value: 0x"); */
/*   //  print_dbg_hex(adc->val2); */
/*   net_activate(adc->outs[2], adc->val2, &(adc->super)); */
/* } */

/* // input value */
/* static void op_adc_in_val3(op_adc_t* adc, const io_t v) { */
/*   // simply passes value to output */
/*   adc->val3 = v; */
/*   //  print_dbg("\r\n adc op output, channel 3, value: 0x"); */
/*   //  print_dbg_hex(adc->val3); */
/*   net_activate(adc->outs[3], adc->val3, &(adc->super)); */
/* } */

// ===== UI input

// increment
static void op_adc_inc_fn(op_adc_t* adc, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0: // enable (toggle)
    if(adc->enable) {
      val = 0;
      op_adc_in_enable(adc, val);
    } else {
      val = OP_ONE;
      op_adc_in_enable(adc, val);
    }
    break;
  case 1: // period
    val = op_sadd(adc->period, inc);
    op_adc_in_period(adc, val);
    break;
  /* case 2: // val0 */
  /*   val = op_sadd(adc->val0, inc); */
  /*   op_adc_in_val0(adc, val); */
  /*   break; */
  /* case 3: // val1 */
  /*   val = op_sadd(adc->val1, inc); */
  /*   op_adc_in_val1(adc, val); */
  /*   break; */
  /* case 4: // val2 */
  /*   val = op_sadd(adc->val2, inc); */
  /*   op_adc_in_val2(adc, val); */
  /*   break; */
  /* case 5: // val3 */
  /*   val = op_sadd(adc->val3, inc); */
  /*   op_adc_in_val3(adc, val); */
  /*   break; */
  }
}

//===== pickles

u8* op_adc_pickle(op_adc_t* adc, u8* dst) {
  dst = pickle_io(adc->enable, dst);
  dst = pickle_io(adc->period, dst);
  return dst;
}

const u8* op_adc_unpickle(op_adc_t* adc, const u8* src) {
  src = unpickle_io(src, &(adc->enable));
  src = unpickle_io(src, &(adc->period));
  if(adc->enable) {
    timers_set_adc(op_to_int(adc->period));    
  }
  return src;
}


void op_adc_sys_input(op_adc_t* adc, u8 ch, u16 val) {
  adc->val[ch] = val;
  net_activate(adc->outs[ch], val, &(adc->super));
}
