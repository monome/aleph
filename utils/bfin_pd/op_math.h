/* op_math.h
 * bees
 *
 * macros for arithmetic in BEES
 */

#ifndef _OP_MATH_H_
#define _OP_MATH_H_

#include "types.h"
#include "fix.h"
#include "pickle.h"

//! input/output data type

typedef s16 io_t;
#define IO_BITS 16
//! shift amount between IO type and 32b
#define IO_SHIFT_32 (32 - IO_BITS)
//! shift amount between IO type and 16b
#define IO_SHIFT_16 (16 - IO_BITS)

//--- pickling
#define pickle_io(src, dst) pickle_16((u16)src, dst);
#define unpickle_io(src, dst) unpickle_16(src, (u16*)dst);

//----- constants
//! unity 
#define OP_ONE 1
//! negative unity
#define OP_NEG_ONE -1

//! smallest incremnet
#define OP_MIN_INC 1
//! ranges
#define OP_MIN_VAL 0 //??
#define OP_MAX_VAL 0x7fff

#define op_from_int(x) (x)
#define op_to_int(x) (x)

//----- conversion

//! standard (overflow) 
extern io_t op_add(io_t a, io_t b);
extern io_t op_sub(io_t a, io_t b);
extern io_t op_mul(io_t a, io_t b);
extern io_t op_div(io_t a, io_t b);

//! saturating
extern io_t op_sadd(io_t a, io_t b);
extern io_t op_ssub(io_t a, io_t b);

//! print formatted string
extern void op_print(char* buf, io_t x);

#endif
