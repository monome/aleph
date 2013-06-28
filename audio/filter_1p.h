/* filters.h
 * null
 * aleph
 *
 * declaration of filtering functions
 */

#include "fix.h"
#include "types.h"

typedef struct _filter_1p {
  u32 sr;    // sample rate
  fix16 c; // decay coefficient
  fix16 x; // target value
  fix16 y; // filtered value
  u8 sync;   // flag if target and filtered values are equal
} filter_1p_fix16;


typedef struct _filter_1p_fr32 {
  u32 sr;    // sample rate
  fract32 c; // decay coefficient
  fract32 x; // target value
  fract32 y; // filtered value
  u8 sync;   // flag if target and filtered values are equal
} filter_1p_fr32;

//=============
//==== fix16

// intialize at pre-allocated memory
extern void filter_1p_fix16_init(   filter_1p_fix16* f, u32 sr, fix16 hz, fix16 in);
// set cutoff frequency in hz
extern void filter_1p_fix16_set_hz( filter_1p_fix16* f, fix16 hz);
// set input value
extern void filter_1p_fix16_in(     filter_1p_fix16* f, fix16 val);
// get next filtered value
extern fix16 filter_1p_fix16_next(  filter_1p_fix16* f);


//==========
//====== fr32
// initialize
extern void filter_1p_fr32_init(filter_1p_fr32* f, u32 sr, fix16 hz, fract32 in);
// set cutoff frequency in hz
extern void filter_1p_fr32_set_hz(filter_1p_fr32* f, fix16 hz);
// set input value
extern void filter_1p_fr32_in(filter_1p_fr32* f, fract32 val);
// get next filtered value
extern fract32 filter_1p_fr32_next(filter_1p_fr32* f);

//==========
//===== s32 (cast and scale)

