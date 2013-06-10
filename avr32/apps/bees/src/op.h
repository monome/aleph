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

// max inputs in a single operator
#define OP_INS_MAX 32
// max outputs in a single operator
#define OP_OUTS_MAX 32

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


//-----------------------------------
//---- public data
extern const op_desc_t op_registry[numOpClasses];

//-----------------------------------
//---- public functions
// initialize operator at memory
extern s16 op_init(op_t* op, opId_t opId);
// get input name
extern const char* op_in_name(op_t* op, const u8 idx);
// get output name
extern const char* op_out_name(op_t* op, const u8 idx);
// get input value
extern io_t op_get_in_val(op_t* op, s16 idx);
// set input valueo
extern void op_set_in_val(op_t* op, s16 idx, const io_t val);

#endif // header guard
