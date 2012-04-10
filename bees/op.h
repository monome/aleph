/* ctl_op.h
 * aleph-avr32
 *
 * base classes implementing arbitrary control flow networks
 * 
 * derived classes need this,
 * other modules should hopefully only need ctlnet_interface.h
 */

#ifndef _BEES_OP_H_
#define _BEES_OP_H_

#include "../common/types.h"

// maximum bytes of operator classes
// WARNING please do not exceed this!
// #define OP_MAX_SIZE 128

//---- operator list enum
typedef enum {
  eOpSwitch,
  eOpEnc,
  eOpAdd,
  eOpMul,
  eOpGate,
  eOpAccum,
  eOpSelect,
  eOpMapLin,
  eOpParam,
  eOpPreset,
  numOpClasses
} opid_t;

//---- op descriptor type
typedef struct op_desc_struct {
  const char* name;
  const U32 size;
} op_desc_t;

//---- input type
// a function pointer to represent an operator's input
// each function is passed a void* to its receiver
// and a pointer to const S32 for input value
typedef void(*op_in_t)(void* rec, const S32* input );

// ---- output type
// an index into the global output table
// a negative index is not evaluated
typedef S16 op_out_t; 

// ---- op_t
// base class for all processors in a control network
// exposes a set of IO points and takes action when (some) inputs are activated
typedef struct op_struct {
  // U16 size;
  U8 numInputs;
  U8 numOutputs;
  // array of function pointers for input
  op_in_t* in;
  // array of pointer indices for output targets.
  op_out_t* out; 
  // name string
  const char* opString;
  // input names concatenated into a single string
  const char* inString;
  // output names concatenated into a single string
  const char* outString;
  // operator type index in registry
  U8 type;
  // offset in statically allocated op memory pool
  //  U32 memOffset;
} op_t;

/// get input name
const char* op_in_name(op_t* op, const U8 idx);
// get output name
const char* op_out_name(op_t* op, const U8 idx);

/// ===== operator subclasses
// each of these structures holds the superclass and private state vairables
// inputs and outputs are described in the _init function defs

//--- op_sw_t : switch
typedef struct op_sw_struct {
  op_t super;
  S32 val; // anything to be used as output needs 4 bytes
  U8 tog;
  op_out_t outs[1];
} op_sw_t;
void op_sw_init(op_sw_t* sw);

//--- op_enc_t : encoder
typedef struct op_enc_struct {
  op_t super;
  U8 pos_now, pos_old;
  S32 val;
  op_out_t outs[1];
} op_enc_t;
void op_enc_init(op_enc_t* sw);

//--- op_add_t : addition
typedef struct op_add_struct {
  op_t super;
  S32 a, b, val;
  U8 btrig;
  op_out_t outs[1];
} op_add_t;
void op_add_init(op_add_t* add);

//--- op_mul_t : multiplication 
typedef struct op_mul_struct {
  op_t super;
  S32 a, b, val;
  U8 btrig;
  op_out_t outs[1];
} op_mul_t;
void op_mul_init(op_mul_t* mul);

//--- op_gate_t : gate
typedef struct op_gate_struct {
  op_t super;
  S32 val;
  U8 gate;
  U8 store;
  op_out_t outs[1];
} op_gate_t;
void op_gate_init(op_gate_t* gate);

//--- op_accum_t : accumulator
typedef struct op_accum_struct {
  op_t super;
  S32 val, min, max;
  U8 carry;
  op_out_t outs[2];
} op_accum_t;
void op_accum_init(op_accum_t* accum);

//--- op_sel_t : range selection 
typedef struct op_sel_struct {
  op_t super;
  S32 min, max;
} op_sel_t;
void op_sel_init(op_sel_t* sel);

//--- op_lin_t : linear map
typedef struct op_lin_struct {
  op_t super;
  S32 inMin, inMax, outMin, outMax, x;
} op_lin_t;
void op_lin_init(op_lin_t* lin);

/*
//--- op_exp_t : exponential map
typedef struct op_exp_struct {
  op_t super;
  S32 inMin, inMax, outMin, outMax, x;
} op_exp_t;
void op_exp_init(op_exp_t* exp);
*/

//--- op_param_t : receive parameter change
typedef struct op_param_struct {
  op_t super;
  U16 idx;
  S32 val;
} op_param_t;
void op_param_init(op_param_t* param);


//--- op_preset_t : preset store / recall
typedef struct op_preset_struct {
  op_t super;
  U16 idx;
} op_preset_t;
void op_preset_init(op_preset_t* preset);

//-----------------------------------
//---- public data
extern const op_desc_t op_registry[numOpClasses];


#endif // header guard
