#include "net_protected.h"
#include "op_adc.h"

//-------------------------------------------------
//----- descriptor
static const char* op_sw_instring = "VAL     ";
static const char* op_adc_outstring = "VAL     ";
static const char* op_adc_opstring = "ADC";

//-------------------------------------------------
//----- static function declaration
static void op_adc_inc_func(op_adc_t* adc, const s16 idx, const io_t inc);
static void op_adc_in_mul(op_adc_t* adc, const io_t* v);

static op_in_func_t op_adc_in_func[3] = {
  (op_in_func_t)&op_adc_in_val,
};


//---------------------------------------------
//----- external function definition

/// initialize
void op_adc_init(op_adc_t* sw) {
  adc->super.numInputs = 3;
  adc->super.numOutputs = 1;
  adc->outs[0] = -1;
  adc->super.inc_func = (op_inc_func)op_adc_inc_func;
  adc->super.in_func = op_adc_in_func;
  adc->super.in_val = adc->in_val;
  adc->in_val[0] = &(adc->val);
  adc->super.out = adc->outs;
  adc->super.opString = op_adc_opstring;
  adc->super.inString = op_adc_instring;
  adc->super.outString = op_adc_outstring;
  adc->super.type = eOpAdc;
  adc->super.flags |= (1 << eOpFlagSys);
  adc->val = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input value
static void op_adc_in_val(op_adc_t* adc, const io_t* v) {
  // simply passes value to output
  adc->val = v;
  net_activate(adc->outs[0], adc->val);
}


//===== UI input

// increment
static void op_adc_inc_func(op_adc_t* adc, const s16 idx, const io_t inc) {
  // no meaningful user input.
}
