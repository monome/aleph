/************************************************************************
 *
 * fract_math.h
 *
 * Copyright (C) 2000-2007 Analog Devices, Inc.
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License. See the file COPYING.LIB for more details.
 *
 * Non-LGPL License is also available as part of VisualDSP++
 * from Analog Devices, Inc.
 *
 ************************************************************************/

/* Definitions of math.h-style functions for fractional types. */

#ifndef _FRACT_MATH_H
#define _FRACT_MATH_H

#include "types.h"

/****************************************/
/*                                      */
/*   fract16 and fract2x16 arithmetic   */
/*                                      */
/****************************************/

/* Adds two packed fracts.
 * Input: _x{a,b} _y{c,d}
 * Returns: {a+c,b+d} */
fract2x16 add_fr2x16(fract2x16 _x, fract2x16 _y);

/* Performs 16-bit addition of the two input parameters (_x+_y) */
fract16 add_fr1x16(fract16 _x, fract16 _y);

/* Subtracts two packed fracts.
 * Input: _x{a,b} _y{c,d}
 * Returns: {a-c,b-d} */
fract2x16 sub_fr2x16(fract2x16 _x, fract2x16 _y);

/* Performs 16-bit subtraction of the two input parameters (_x-_y) */
fract16 sub_fr1x16(fract16 _x, fract16 _y);

/* Multiplies two packed fracts.  Truncates the results to 16 bits.
 * Inputs: _x{a,b} _y{c,d}
 * Returns: {trunc16(a*c),trunc16(b*d)} */
fract2x16 mult_fr2x16(fract2x16 _x, fract2x16 _y);

/* Performs 16-bit multiplication of the input parameters (_x*_y). 
 * The result is truncated to 16 bits. */
fract16 mult_fr1x16(fract16 _x, fract16 _y);

/* Multiplies two packed fracts.  Rounds the result to 16 bits.  Whether the 
 * rounding is biased or unbiased depends on what the RND_MOD bit in the ASTAT 
 * register is set to.
 * Input: _x{a,b} _y{c,d}
 *  Returns: {round16{a*c},round16{b*d}} */
fract2x16 multr_fr2x16(fract2x16 _x, fract2x16 _y);

/* Performs a 16-bit fractional multiplication (_x*_y) of the two input 
 * parameters. The result is rounded to 16 bits. Whether the rounding is biased
 * or unbiased depends what the RND_MOD bit in the ASTAT register is set to. */
fract16 multr_fr1x16(fract16 _x, fract16 _y) ;

/* Negates both 16 bit fracts in the packed fract.  If one of the fract16 values 
 * is 0x8000, saturation occurs and 0x7fff is the result of the negation.
 * Input: _x{a,b}
 * Returns: {-a,-b} */
fract2x16 negate_fr2x16(fract2x16 _x);

/* Returns the 16-bit result of the negation of the input parameter (-_x).  If 
 * the input is 0x8000, saturation occurs and 0x7fff is returned. */
fract16 negate_fr1x16(fract16 _x);

/* Arithmetically shifts both fract16s in the fract2x16 left by _y places,and 
 * returns the packed result.  The empty bits are zero filled.  If shft is 
 * negative, the shift is to the right by abs(shft) places with sign extension.
 * 
 * WARNING: All the bits except for the lowest 5 bits of
 * _y are ignored.  If you want to shift by numbers larger than 5 bit signed ints,
 * you should use shl_fr2x16_clip.
 *
 * Inputs: _x{a,b} _y
 * Returns: {a<<shft,b<<shft} */
fract2x16 shl_fr2x16(fract2x16 _x, int _y);

/* Arithmetically shifts both fract16s in the fract2x16 left by _y places,and 
 * returns the packed result.  The empty bits are zero filled.  If shft is 
 * negative, the shift is to the right by abs(_y) places with sign extension.
 * 
 * This avoids the limitation whereby all the bits 
 * except the lowest 5 bits of _y are ignored.
 *
 * Inputs: _x{a,b} _y
 * Returns: {a<<_y,b<<_y} 
 */
fract2x16 shl_fr2x16_clip(fract2x16 _x, int _y);

/* Arithmetically shifts the src variable left by _y places.  The empty bits 
 * are zero filled. If shft is negative, the shift is to the right by abs(_y)
 * places with sign extension.
 *
 * WARNING: All the bits except for the lowest 5 bits
 * of _y are ignored.  To shift by numbers larger than 5 bit signed
 * ints, shl_fr1x16_clip should be used. 
 */
fract16 shl_fr1x16(fract16 _x, int _y);

/* Arithmetically shifts the src variable left by _y places.  The empty bits 
 * are zero filled. If shft is negative, the shift is to the right by abs(shft)
 * places with sign extension.
 *
 * This clipping variant allows shifts of numbers that are too big to be
 * represented in 5 bits. 
 */
fract16 shl_fr1x16_clip(fract16 _x, int _y);

/* Arithmetically shifts both fract16s in the fract2x16 right by shft places 
 * with sign extension, and returns the packed result.  If shft is negative, 
 * the shift is to the left by abs(shft) places, and the empty bits are zero 
 * filled.
 *
 * WARNING: All the bits except for the lowest 5 bits of
 * _y are ignored.  If you want to shift by numbers larger than 5 bit signed ints,
 * you should use shr_fr2x16_clip.
 *
 * Inputs: _x{a,b} _y
 * Returns: {a<<shft,b<<shft} 
 */
fract2x16 shr_fr2x16(fract2x16 _x, int _y);

/* Arithmetically shifts both fract16s in the fract2x16 right by shft places 
 * with sign extension, and returns the packed result.  If shft is negative, 
 * the shift is to the left by abs(_y) places, and the empty bits are zero 
 * filled.
 *
 * This clipping variant allows shifts of numbers that are too big to be
 * represented in 5 bits. 
 */
fract2x16 shr_fr2x16_clip(fract2x16 _x, int _y);

/* Arithmetically shifts the src variable right by shft places with sign 
 * extension.  If shft is negative, the shift is to the left by abs(_y) places, 
 * and the empty bits are zero filled.
 *
 * WARNING: All the bits except for the lowest 5 bits of
 * _y are ignored.  If you want to shift by numbers larger than 5 bit signed ints,
 * you should use shr_fr1x16_clip.
 */
fract16 shr_fr1x16(fract16 _x, int _y);

/* Arithmetically shifts the src variable right by shft places with sign 
 * extension.  If shft is negative, the shift is to the left by abs(_y) places, 
 * and the empty bits are zero filled.
 *
 * This clipping variant allows shifts of numbers that are too big to be
 * represented in 5 bits. 
 */
fract16 shr_fr1x16_clip(fract16 _x, int _y);


/* Logically shifts both fract16s in the fract2x16 right by shft places.  There
 * is no sign extension and no saturation - the empty bits are zero filled.
 *
 * WARNING: All the bits except for the lowest 5 bits of
 * _y are ignored.  If you want to shift by numbers larger than 5 bit signed ints,
 * you should use shrl_fr2x16_clip.
 */
fract2x16 shrl_fr2x16(fract2x16 _x, int _y);

/* Logically shifts both fract16s in the fract2x16 right by shft places.  There
 * is no sign extension and no saturation - the empty bits are zero filled.
 *
 * This clipping variant allows shifts of numbers that are too big to be
 * represented in 5 bits. 
 */
fract2x16 shrl_fr2x16_clip(fract2x16 _x, int _y);


/* Logically shifts a fract16 right by shft places.  There is no sign extension
 * and no saturation - the empty bits are zero filled.
 *
 * WARNING: All the bits except for the lowest 5 bits of
 * _y are ignored.  If you want to shift by numbers larger than 5 bit signed ints,
 * you should use shrl_fr1x16_clip.
 */
fract16 shrl_fr1x16(fract16 _x, int _y);

/* Logically shifts a fract16 right by _x places.  There is no sign extension
 * and no saturation - the empty bits are zero filled.
 *
 * This clipping variant allows shifts of numbers that are too big to be
 * represented in 5 bits. 
 */
fract16 shrl_fr1x16_clip(fract16 _x, int _y);



/**************************/
/*                        */
/*   fract32 arithmetic   */
/*                        */
/**************************/

/* Performs 32-bit addition of the two input parameters (_x+_y). */
fract32 add_fr1x32(fract32 _x, fract32 _y);

/* Performs 32-bit subtraction of the two input parameters (_x-_y). */
fract32 sub_fr1x32(fract32 _x, fract32 _y);

/* Performs 32-bit multiplication of the input parameters (_x*_y).  
 * The result (which is calculated internally with an accuracy of 40 bits) 
 * is rounded (biased rounding) to 32 bits.  */
fract32 mult_fr1x32x32(fract32 _x, fract32 _y);

/* Performs 32-bit non-saturating multiplication of the input parameters 
 * (_x*_y). This is somewhat faster than mult_fr1x32x32. The result (which is 
 * calculated internally with an accuracy of 40 bits) is rounded (biased 
 * rounding) to 32 bits.  */
fract32 mult_fr1x32x32NS(fract32 _x, fract32 _y);

/* Returns the 32-bit value that is the absolute value of the input parameter.
 * Where the input is 0x80000000, saturation occurs and 0x7fffffff is returned.
 */
fract32 abs_fr1x32(fract32 _x);

/* Returns the 32-bit result of the negation of the input parameter (-_x).  If
 * the input is 0x80000000, saturation occurs and 0x7fffffff is returned.  */
fract32 negate_fr1x32(fract32 _x);

/* Returns the minimum of the two input parameters */
fract32 min_fr1x32(fract32 _x, fract32 _y);

/* Returns the maximum of the two input parameters */
fract32 max_fr1x32(fract32 _x, fract32 _y);

/* Arithmetically shifts the src variable left by shft places.  The empty bits 
 * are zero filled. If shft is negative, the shift is to the right by abs(shft)
 * places with sign extension.
 *
 * WARNING: All the bits except for the lowest 6 bits of
 * _y are ignored.  If you want to shift by numbers larger than 6 bit signed ints,
 * you should use shl_fr1x32_clip.
*/
fract32 shl_fr1x32(fract32 _x, int _y);

/* Arithmetically shifts the src variable left by shft places.  The empty bits 
 * are zero filled. If shft is negative, the shift is to the right by abs(_y)
 * places with sign extension.
 *
 * This clipping variant allows shifts of numbers that are too big to be represented
 * in 6 bits.
*/
fract32 shl_fr1x32_clip(fract32 _x, int _y);

/* Arithmetically shifts the src variable right by shft places with sign 
 * extension.  If shft is negative, the shift is to the left by abs(_y) places, 
 * and the empty bits are zero filled.
 *
 * WARNING: All the bits except for the lowest 6 bits of
 * _y are ignored.  If you want to shift by numbers larger than 6 bit signed ints,
 * you should use shr_fr1x32_clip.
*/
fract32 shr_fr1x32(fract32 _x, int _y);

/* Arithmetically shifts the src variable right by shft places with sign 
 * extension.  If shft is negative, the shift is to the left by abs(_y) places, 
 * and the empty bits are zero filled.
 *
 * This clipping variant allows shifts of numbers that are too big to be represented
 * in 6 bits.
*/
fract32 shr_fr1x32_clip(fract32 _x, int _y);

/* Performs a fractional multiplication on two 16-bit fractions, returning the 
 * 32-bit result. There will be loss of precision. */
fract32 mult_fr1x32(fract16 _x, fract16 _y);

/*   Function
 *    long fract24_8mul_asm(fract32 A, fract32 B);
 *
 *   Description   : calculates 24.8 Fractional multiplication
 *    fract32 A       : 24.8 fractional number
 *    fract32 B       : 24.8 fractional number
 *    return-value : 24.8 fractional number
*/

fract32 fract24_8mul_asm(fract32, fract32);


/*   Function
 *    long fract28_4mul_asm(fract32 A, fract32 B);
 *
 *   Description   : calculates 28.4 Fractional multiplication
 *    fract32 A       : 28.4 fractional number
 *    fract32 B       : 28.4 fractional number
 *    return-value : 28.4 fractional number
*/

fract32 fract28_4mul_asm(fract32, fract32);

/*****************************/
/*                           */
/*   Conversion operations   */
/*                           */
/*****************************/

/* If _x>0x00007fff, returns 0x7fff. If _x<0xffff8000, returns 0x8000.
   Otherwise returns the lower 16 bits of _x. */
fract16 sat_fr1x32(fract32 _x);

/* This function rounds the 32-bit fract to a 16-bit fract using biased 
 * rounding.  */
fract16 round_fr1x32(fract32 _x);

/* Returns the number of left shifts required to normalize the input variable 
 * so that it is either in the interval 0x40000000 to 0x7fffffff, or in the 
 * interval 0x80000000 to 0xc0000000.  In other words,
 *  
 *  fract32 x;
 *  shl_fr1x32(x,norm_fr1x32(x));
 *
 * will return a value in the range 0x40000000 to 0x7fffffff, or in the range 
 * 0x80000000 to 0xc0000000.
 */
int norm_fr1x32(fract32 _x);

/* Returns the number of left shifts required to normalize the input variable 
 * so that it is either in the interval 0x4000 to 0x7fff, or in the interval
 * 0x8000 to 0xc000. In other words,
 *
 *  fract16 x;
 *  shl_fr1x16(x,norm_fr1x32(x));
 *
 * will return a value in the range 0x4000 to 0x7fff, or in the range 0x8000 
 * to 0xc000.
 */
int norm_fr1x16(fract16 _x); 

/* This function returns the top 16 bits of _x, i.e. it truncates _x to 16
 * bits.  */
fract16 trunc_fr1x32(fract32 _x);

/* Returns the 16-bit value that is the absolute value of the input parameter.
 * Where the input is 0x8000, saturation occurs and 0x7fff is returned. */
fract16 abs_fr16 (fract16 _x);

/* Returns the 16-bit value that is the absolute value of the input parameter.
 * Where the input is 0x8000, saturation occurs and 0x7fff is returned. */
fract16 abs_fr1x16 (fract16 _x);

/* Returns the maximum of the two input parameters. */
fract16 max_fr16 (fract16 _x, fract16 _y);

/* Returns the maximum of the two input parameters. */
fract16 min_fr16 (fract16 _x, fract16 _y);

fract16 max_fr1x16 (fract16 _x, fract16 _y);

/* Returns the maximum of the two input parameters. */
fract16 min_fr1x16 (fract16 _x, fract16 _y);

void* __builtin_bfin_circptr(void* ptr, long unsigned int wordLength, void* buf, long unsigned int bufLength);

#endif /* _FRACT_MATH_H */
