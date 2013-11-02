#include "net_protected.h"
#include "op_adc.h"

//-------------------------------------------------
//----- descriptor
static const char* op_sw_instring	= "VAL0    VAL1    VAL2    VAL3    ";
static const char* op_adc_outstring	= "VAL0    VAL1    VAL2    VAL3    ";
static const char* op_adc_opstring	= "ADC";

//-------------------------------------------------
//----- static function declaration
static void op_adc_inc_input	(op_adc_t* adc, const s16 idx, const io_t inc);
static void op_adc_in_val0	(op_adc_t* adc, const io_t* v);
static void op_adc_in_val1	(op_adc_t* adc, const io_t* v);
static void op_adc_in_val2	(op_adc_t* adc, const io_t* v);
static void op_adc_in_val3	(op_adc_t* adc, const io_t* v);

static op_in_fn op_adc_in_fn[4] = {
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
void op_adc_init(op_adc_t* sw) {
  adc->super.numInputs = 0;
  adc->super.numOutputs = 4;
  adc->outs[0] = -1;
  adc->outs[1] = -1;
  adc->outs[2] = -1;
  adc->outs[3] = -1;
  //  adc->super.inc_fn = (op_inc_fn)op_adc_inc_fn;
  adc->super.in_fn = op_adc_in_fn;
  adc->super.in_val = adc->in_val;
  //  adc->in_val[0] = &(adc->val);
  adc->super.out = adc->outs;
  adc->super.opString = op_adc_opstring;
  adc->super.inString = op_adc_instring;
  adc->super.outString = op_adc_outstring;
  adc->super.type = eOpAdc;
  // user creates 4-channel adc operator when adc input is desired
  //  adc->super.flags |= (1 << eOpFlagSys);
  adc->val = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input value
static void op_adc_in_val0(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val0 = *v;
  net_activate(adc->outs[0], adc->val0);
}

// input value
static void op_adc_in_val1(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val1 = *v;
  net_activate(adc->outs[1], adc->val1);
}

// input value
static void op_adc_in_val2(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val2 = *v;
  net_activate(adc->outs[2], adc->val2);
}

// input value
static void op_adc_in_val3(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val3 = *v;
  net_activate(adc->outs[3], adc->val3);
}

// ===== UI input

// increment
static void op_adc_inc_input(op_adc_t* adc, const s16 idx, const io_t inc) {
  // no meaningful user input.
}

//===== pickles

u8* op_adc_pickle(op_adc_t* adc, u8* dst) {
  ;;  // no state
}

const u8* op_adc_unpickle(op_adc_t* adc, const u8* src) {
  ;; // no state 
}
