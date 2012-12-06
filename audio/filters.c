/* filters.c
 * null
 * aleph
 */

// std
#include <math.h>
//#include <cmath>
// aleph
#include "filters.h"


#ifdef ARCH_BFIN
#include "fract_math.h"
#include <fract2float_conv.h>
#else
#include "fract32_emu.h"
#endif

//#define FR32_COMP_THRESH 0x7
#define FIX16_COMP_THRESH 0x10000
#define FIX16_COMP_THRESH_NEG 0xfffeffff

#define FR32_COMP_THRESH 0x7
#define FR32_COMP_THRESH_NEG 0xfffffffe

//-----------------------
//---- static functions

static inline u8 fr32_compare(fract32 a, fract32 b) {
  fract32 dif = sub_fr1x32(a, b);
  return ( ( dif < FR32_COMP_THRESH ) && ( dif > FR32_COMP_THRESH_NEG ) );
  //  return ( abs_fr1x32(sub_fr1x32(a, b)) > FR32_COMP_THRESH );
}


static inline u8 fix16_compare(fix16 a, fix16 b) {
  //  return ( abs_fr1x32(sub_fr1x32(a, b)) > FIX16_COMP_THRESH );
  fix16 dif = fix16_sub(a, b);
  return ( ( dif < FIX16_COMP_THRESH ) && ( dif > FIX16_COMP_THRESH_NEG ) );
}


//-----------------------
//---- external functions

//===============================
//==============================
//===== fix16

// intialize at pre-allocated memory
void filter_1p_fix16_init(filter_1p_fix16* f, u32 sr, fix16 hz, fix16 in) {
  f->c = 0;
  f->y = in;
  f->x = in;
  f->sync = 1;
  f->sr = sr;
  filter_1p_fix16_set_hz(f, hz);
}

// set cutoff frequency in hz
void filter_1p_fix16_set_hz(filter_1p_fix16* f, fix16 hz) { 
  //// debug
  //f32 fc =  (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) / (float)(f->sr) );
   //  printf("\r1p coefficient: %f\n", fc);
  /// /FIXME: could optimize
  f->c = fix16_from_float( (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) / (float)(f->sr) ) ); 
  // clamp to < 1.0
  if (f->c > 0xffff) {  f->c = 0xffff; }
  //  printf("\rfix16: %08x\n",  f->c );
}

// set target value 
void filter_1p_fix16_in(filter_1p_fix16* f, fix16 val) {
  f->x = val;
  f->sync = (val == f->y);
}

// get next filtered value

fix16 filter_1p_fix16_next(filter_1p_fix16* f) {
  if(f->sync) {
    ;;
  } else {
    f->y = fix16_add( f->x,
		      fix16_mul(f->c,
				fix16_sub(f->y, f->x)
				));
    if(fix16_compare(f->x, f->y)) {
      f->y = f->x;
      f->sync = 1;
    }
  }
  return f->y;
}


//===============================
//==============================
//===== fr32

// intialize at pre-allocated memory
void filter_1p_fr32_init(filter_1p_fr32* f, u32 sr, fix16 hz, fract32 in) {
  f->c = 0;
  f->y = in;
  f->x = in;
  f->sync = 1;
  f->sr = sr;
  filter_1p_fr32_set_hz(f, hz);
}

// set cutoff frequency in hz
void filter_1p_fr32_set_hz(filter_1p_fr32* f, fix16 hz) {
  //// debug
  f32 fc =  (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) / (float)(f->sr) );
  //  printf("\r1p coefficient: %f\n", fc);
  //f->c = float_to_fr32( (float) exp(-2.0 * M_PI * (double)(fix16_to_float(hz)) / (double)(f->sr) ) ); 
  f->c = float_to_fr32(fc);
}

// set target value 
void filter_1p_fr32_in(filter_1p_fr32* f, fract32 val) {
  f->x = val;
  f->sync = (val == f->y);
}

// get next filtered value
fract32 filter_1p_fr32_next(filter_1p_fr32* f) {
  
  if(f->sync) {
    ;;
  } else {
    f->y = add_fr1x32( f->x,
		       mult_fr1x32x32(f->c,
				      sub_fr1x32(f->y, f->x)
				      ));
    if(fr32_compare(f->x, f->y)) {
      f->y = f->x;
      f->sync = 1;
    }
  }
  return f->y;
}

