#include "fix32.h"

// add 32.32 value with overflow/underflow checking
void add_fix32(fix32* a, fix32* b) {
  // tmp fract
  fract32 tfr = a->fr;
  // tmp int
  s32 ti = a->i + b->i;
 
  /// FIXME: could be arch-specific inline ASM here for better speed
  /// (e.g. add fract32, check overflow flag ?)
  if(tfr >= 0) {
    if( sub_fr1x32(FRACT32_MAX,tfr) < b->fr) {
      // wrap by subtraction
      tfr = sub_fr1x32(
			 add_fr1x32(
				    sub_fr1x32(tfr, FR32_MAX),
				    b->fr),
			 FR32_MAX);
      ti += 1; // carry
    }
  } else {
    if(b->fr < sub_fr1x32(FRACT32_MAX,tfr)) {
      // wrap by addition
      tfr = add_fr1x32(
			 add_fr1x32(
				    add_fr1x32(tfr, FR32_MAX),
				    b->fr),
			 FR32_MAX);
      ti -= 1; // carry (negative)
      
    }
  }
  // yet another comparison and carry for negative fr
  if(tfr < 0) {
    a->fr = add_fr1x32(FR32_MAX, tfr);
    a->i = ti -1;
  } else {
    a->fr = tfr;
    a->i = ti;
  }
}

// subtract with over/underflow checking
void sub_fix32(fix32* a, fix32* b) {
  fix32 bTmp;
  bTmp.i = BIT_INVERT(b->i);
  add_fix32(a, &bTmp);
}


// convert
void fix16_to_fix32(fix16* in, fix32* out) {
  out->fr = FIX16_FRACT_TRUNC(*in);
  out->i = (s32)(FIX16_TO_S16(*in));
}


// wrap a 32.32 value to a positive integer range
extern void fix32_wrap_range(fix32* a, u32 upperBound) {
  static fix32 upFix;
  upFix.i = upperBound;
  upFix.fr = 0;
  //  u8 wrapCount = 0;
  while (a->i < 0) {
    //a->i += upperBound;
    add_fix32(a, &upFix);
      //    wrapCount++;
  }
  //  if(wrapCount) {
    //    a->fr = sub_fr1x32(FR32_MAX, a->fr);
  //  }
  while(a->i > (upperBound-1)) {
    // a->i -= upperBound;
    sub_fix32(a, &upFix);
  }
}
