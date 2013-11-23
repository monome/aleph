/* scaler.h
   bees

   parameter-scaling module.

   a scaler converts io_t to full-scale 4-byte parameter representation (native DSP value.) 
   sometimes this can be done by calculation, more often i think by tuned tabled lookup.

   each scaler type must maintain a lookup table size.
   will try and keep these flexible enough that tuning can be performed offline on table data,
   and hopefully from the aleph UI at some point.

   in addition to a DSP use value, 
   scaler must provide human-readable value as well (in 16.16 i guess)
   so, two tables per initialized parameter type.
   
*/

#ifndef _ALEPH_AVR32_BEES_SCALER_H_
#define _ALEPH_AVR32_BEES_SCALER_H_


#include "op_math.h"
#include "param_common.h"

// initialize scaler for given type
extern void scaler_init(ParamType type, fix16 min, fix16 max);
// get DSP value for given type, input
extern void scaler_get_value(ParamType type, io_t in);
// get readable value for given type, input
extern void scaler_get_rep(ParamType type, io_t in);

#endif
