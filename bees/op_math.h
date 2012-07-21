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


// input/output data type
typedef f32 io_t;

// arithmetic macros
#define OPADD(a, b) ((a) + (b))
#define OPSUB(a, b) ((a) - (b))
#define OPMUL(a, b) ((a) * (b))
#define OPDIV(a, b) ((a) / (b))

#endif
