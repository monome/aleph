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

typedef s16 io_t;
#define IO_BITS 16

//--- pickling
//#define pickle_io(src, dst) pickle_32((u32)src, dst);
//#define unpickle_io(src, dst) unpickle_32(src, (u32*)dst);
#define pickle_io(src, dst) pickle_16((u16)src, dst);
#define unpickle_io(src, dst) unpickle_16(src, (u16*)dst);

//---- arithmetic macros
// standard (overflow)
/* #define OP_ADD(a, b) (fix16_add((a), (b))) */
/* #define OP_SUB(a, b) (fix16_sub((a), (b))) */
/* #define OP_MUL(a, b) (fix16_mul((a), (b))) */
/* #define OP_DIV(a, b) (fix16_div((a), (b))) */
// saturating
/* #define OP_SADD(a, b) (fix16_sadd((a), (b))) */
/* #define OP_SSUB(a, b) (fix16_ssub((a), (b))) */
/* #define OP_SMUL(a, b) (fix16_smul((a), (b))) */
/* #define OP_SDIV(a, b) (fix16_sdiv((a), (b))) */

// standard (overflow)
#define OP_ADD(a, b) ((a) + (b))
#define OP_SUB(a, b) ((a) - (b))
#define OP_MUL(a, b) ((a) * (b))
#define OP_DIV(a, b) ((a) / (b))

// saturating
///// FIXME! probably need inline ASM
// satadd.w 	Rd, Rx, Ry 
// mulsatwh.w 	Rd, Rx, Ry 
// etc
///// for now, ignore! haha good joke
#define OP_SADD(a, b) OP_ADD(a,b)
#define OP_SSUB(a, b) OP_SUB(a,b)
#define OP_SMUL(a, b) OP_MUL(a,b)
#define OP_SDIV(a, b) OP_DIV(a,b)

//----- constants
// unity 
#define OP_ONE 1
// negative unity
#define OP_NEGONE -1

// smallest incremnet
#define OP_MIN_INC 1
// ranges
#define OP_MIN_VAL 0
#define OP_MAX_VAL 0x7fff

#define OP_FROM_INT(x) (x)
#define OP_TO_INT(x) (x)

//----- conversion
///// print formatted string
#define OP_PRINT(buf, x) itoa_whole((s32)(x), (buf), 6)

#endif
