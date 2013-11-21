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

// input/output data type
// typedef f32 io_t;
// typedef fix16_t io_t;
typedef s16 io_t;

//--- pickling
//#define pickle_io(src, dst) pickle_32((u32)src, dst);
//#define unpickle_io(src, dst) unpickle_32(src, (u32*)dst);
#define pickle_io(src, dst) pickle_32((u32)src, dst);
#define unpickle_io(src, dst) unpickle_32(src, (u32*)dst);

//---- arithmetic macros
// standard (overflow)
#define OP_ADD(a, b) (fix16_add((a), (b)))
#define OP_SUB(a, b) (fix16_sub((a), (b)))
#define OP_MUL(a, b) (fix16_mul((a), (b)))
#define OP_DIV(a, b) (fix16_div((a), (b)))
// saturating
#define OP_SADD(a, b) (fix16_sadd((a), (b)))
#define OP_SSUB(a, b) (fix16_ssub((a), (b)))
#define OP_SMUL(a, b) (fix16_smul((a), (b)))
#define OP_SDIV(a, b) (fix16_sdiv((a), (b)))

//----- constants
// unity 
#define OP_ONE fix16_one
// smallest incremnet
#define OP_MIN_INC 0x1
#define OP_MIN_VAL fix16_min
#define OP_MAX_VAL fix16_max

#define OP_FROM_INT(x) (fix16_from_int(x))
#define OP_TO_INT(x) (fix16_to_int(x))

//----- conversion

///// print formatted string
#define OP_PRINT(buf, x) print_fix16( (buf), (x) )

#endif
