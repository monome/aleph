/* ctl_op.h
 * aleph-avr32
 *
 * base classes implementing arbitrary control flow networks
 * 
 * derived classes need this,
 * other modules should hopefully only need the interface header (net.h)
 */

#ifndef _BEES_OP_H_
#define _BEES_OP_H_

#include "types.h"
#include "op_math.h"

//---- operator list enum
typedef enum {
  eOpSwitch,
  eOpEnc,
  eOpAdd,
  eOpMul,
  eOpGate,
  //  eOpAccum,
  //  eOpSelect,
  //  eOpMapLin,
  //  eOpParam,
  //  eOpPreset,
  numOpClasses // dummy/count 
} opId_t;

//---- operator status enum
typedef enum {
  eUserOp = 0,    // user-created
  eSysCtlOp,  // system controller (e.g. encoder)
  eSysRecOp,  // system receiver (e.g. parameter)
  numOpStatuses // dummy/count
} opStatus_t;

//---- op descriptor type
typedef struct op_desc_struct {
  const char* name;
  const u32 size;
} op_desc_t;

//---- input type
// a function pointer to represent an operator's input
// each function is passed a void* to its receiver
// and a pointer to const s32 for input value
typedef void(*op_in_func_t)(void* op, const io_t* input );
typedef void(*op_inc_func)(void* op, const s16 idx, const io_t inc);
// ---- output type
// an index into the global output table
// a negative index is not evaluated
typedef s16 op_out_t; 

// ---- op_t
// base class for all processors in a control network
// exposes a set of IO points and takes action when (some) inputs are activated
typedef struct op_struct {
  // u16 size;
  u8 numInputs;
  u8 numOutputs;
  // input increment function (for UI)
  op_inc_func inc_func;
  // array of input function pointers
  op_in_func_t* in_func;
  // dynamic array of pointers to input values
  io_t ** in_val;
  // array of indices for output targets.
  op_out_t * out; 
  // name string
  const char* opString;
  // input names concatenated into a single string
  const char* inString;
  // output names concatenated into a single string
  const char* outString;
  // operator type index in registry
  u8 type;
  // user/system status
  opStatus_t status;
  // offset in statically allocated op memory pool
  //  u32 memOffset;
} op_t;

/// get input name
const char* op_in_name(op_t* op, const u8 idx);
// get output name
const char* op_out_name(op_t* op, const u8 idx);
// get input value
io_t op_get_in_val(op_t* op, s16 idx);
// set input valueo
void op_set_in_val(op_t* op, s16 idx, const io_t val);

/// ===== operator subclasses
// each of these structures holds the superclass and private state vairables
// inputs and outputs are described in the _init function defs



//// TODO: need work
/*
//--- op_accum_t : accumulator
typedef struct op_accum_struct {
  op_t super;
  io_t val, min, max, wrap;
  op_out_t outs[2];
} op_accum_t;
void op_accum_init(op_accum_t* accum);
*/

/*
//--- op_sel_t : range selection 
typedef struct op_sel_struct {
  op_t super;
  io_t min, max;
} op_sel_t;
void op_sel_init(op_sel_t* sel);
*/

/*
//--- op_lin_t : linear map
typedef struct op_lin_struct {
  op_t super;
  io_t inMin, inMax, outMin, outMax, x;
} op_lin_t;
void op_lin_init(op_lin_t* lin);
*/

/*
//--- op_exp_t : exponential map
typedef struct op_exp_struct {
  op_t super;
  io_t inMin, inMax, outMin, outMax, x;
} op_exp_t;
void op_exp_init(op_exp_t* exp);
*/

/*
//--- op_param_t : receive parameter change
typedef struct op_param_struct {
  op_t super;
  u32 idx;
  io_t val;
} op_param_t;
void op_param_init(op_param_t* param);
*/

//-----------------------------------
//---- public data
extern const op_desc_t op_registry[numOpClasses];

#endif // header guard
