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
#include "fix16.h"

//#define OP_IO_FLOAT 1
#define OP_IO_FIX16 1

// input/output data type
// typedef f32 io_t;
typedef fix16_t io_t;

// arithmetic macros
// #define OPADD(a, b) ((a) + (b))
// #define OPSUB(a, b) ((a) - (b))
// #define OPMUL(a, b) ((a) * (b))
// #define OPDIV(a, b) ((a) / (b))

#define OPADD(a, b) (fix16_add((a), (b)))
#define OPSUB(a, b) (fix16_sub((a), (b)))
#define OPSADD(a, b) (fix16_sadd((a), (b)))
#define OPSSUB(a, b) (fix16_ssub((a), (b)))
#define OPMUL(a, b) (fix16_mul((a), (b)))
#define OPDIV(a, b) (fix16_div((a), (b)))
#define OPSMUL(a, b) (fix16_smul((a), (b)))
#define OPSDIV(a, b) (fix16_sdiv((a), (b)))



#endif
