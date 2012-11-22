/* filters.h
 * null
 * aleph
 *
 * declaration of filtering functions
 */

#include "fix.h"
#include "types.h"

typedef struct _filter_1p {
  fix16 c; // decay coefficient
  fix16 x; // target value
  fix16 y; // filtered value
  u8 sync;   // flag if target and filtered values are equal
} filter_1p;

// intialize at pre-allocated memory
extern void filter_1p_init(filter_1p* f);
// set cutoff frequency in hz
extern void filter_1p_set_hz(filter_1p* f, fix16 hz);
// set target value 
extern void filter_1p_set(filter_1p* f, fix16 val);
// get next filtered value
extern fract32 filter_1p_next(filter_1p* f);
