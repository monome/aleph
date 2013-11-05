/* op_math.h
 * bees
 *
 * we define macros for arithmetic in BEES
 * this is to allow for (relatively) painless migration
 * between numerical representations
 */

#ifndef _OP_MATH_H_
#define _OP_MATH_H_

#include "types.h"
#include "fix.h"

//#define OP_IO_FLOAT 1
#define OP_IO_FIX16 1

// input/output data type
// typedef f32 io_t;
typedef fix16_t io_t;

// arithmetic macros

// #define OP_ADD(a, b) ((a) + (b))
// #define OP_SUB(a, b) ((a) - (b))
// #define OP_MUL(a, b) ((a) * (b))
// #define OP_DIV(a, b) ((a) / (b))

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
// constants
// unity 
#define OP_ONE fix16_one
// smallest incremnet
#define OP_MIN_INC 0x1
#define OP_MIN_VAL fix16_min
#define OP_MAX_VAL fix16_max

///// print formatted string
#define OP_PRINT(buf, x) print_fix16( (buf), (x) )

#endif
