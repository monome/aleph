/* param_scaler.h
   bees

   parameter-scaling module.

   a scaler converts io_t to full-scale 4-byte parameter value  (native DSP unit.) 
   sometimes this can be done by calculation, more often i think by tuned tabled lookup.

   each scaler type must maintain a lookup table size.
   will try and keep these flexible enough that tuning can be performed offline on table data,
   and hopefully from the aleph UI at some point.

   in addition to a DSP use value, 
   scaler must provide human-readable value as well (in 16.16)
   so, two tables per initialized parameter type, 
   unless a simple mapping function exists (e.g. fixed point linear.)
   
*/

#ifndef _ALEPH_AVR32_BEES_SCALER_H_
#define _ALEPH_AVR32_BEES_SCALER_H_


#include "op_math.h"
#include "param_common.h"

// type-specific includes
/// need these for data size calculation
#include "scalers/scaler_amp.h"
#include "scalers/scaler_bool.h"
#include "scalers/scaler_fix.h"
#include "scalers/scaler_fract.h"
#include "scalers/scaler_integrator.h"
#include "scalers/scaler_integrator_short.h"
#include "scalers/scaler_note.h"
#include "scalers/scaler_short.h"
#include "scalers/scaler_svf_fc.h"

//----------------------
//---- defines
#define PARAM_SCALER_DATA_SIZE (PARAM_SCALER_AMP_DATA_SIZE	\
				+ PARAM_SCALER_BOOL_DATA_SIZE	\
				+ PARAM_SCALER_FIX_DATA_SIZE	\
				+ PARAM_SCALER_FRACT_DATA_SIZE	\
				+ PARAM_SCALER_INTEGRATOR_DATA_SIZE	\
				+ PARAM_SCALER_INTEGRATOR_SHORT_DATA_SIZE	\
				+ PARAM_SCALER_NOTE_DATA_SIZE		\
				+ PARAM_SCALER_SHORT_DATA_SIZE \
				+ PARAM_SCALER_SVF_FC_DATA_SIZE )
// + ...

//---------------------
//-- function types
// get DSP value for given scaler, input
typedef s32 (*scaler_get_value_fn)(void* scaler, io_t in);
// print human-readable value to string
typedef void (*scaler_get_str_fn)(char* dst, void* scaler, io_t in);
// search for closest input to DSP value
// - this will tend to be slow, use sparingly in realtime contexts
typedef io_t (*scaler_get_in_fn)(void* scaler, s32 value);
// perform a tuning routine
typedef s32 (*scaler_tune_fn)(void* scaler, u8 tuneId, io_t in);
// use a scaler to increment input type
typedef s32 (*scaler_inc_fn)(void* scaler, io_t *pin, io_t inc);

// class structure i
typedef struct _paramScaler { 
  //// not using these because retarded
   // get value
  //  scaler_get_value_fn get_val;
  // get ui representation
  //  scaler_get_str_fn get_str;

  // param desc pointer
  const ParamDesc *desc;
  // input ranges
  io_t inMin;
  io_t inMax;

  //// TODO, perhapsb
  // array of tuning functions
  //  scaler_tune_fn * tune;
  // num tuning functions
  //  u8 numTune;
  
} ParamScaler;

// initialize scaler for given param (protected, derived)
typedef void (*scaler_init_fn)(void* scaler);

// initialize scaler for given param (public, abstract)
extern void scaler_init(ParamScaler* sc, const ParamDesc* desc);

// get DSP value 
extern s32 scaler_get_value(ParamScaler* sc, io_t in);
// print readable value to string bffer
extern void scaler_get_str(char* dst, ParamScaler* sc, io_t in);
// get input given DSP value (use sparingly)
extern io_t scaler_get_in(ParamScaler* sc, s32 value);
// increment input by pointer, return value
extern s32 scaler_inc(ParamScaler* sc, io_t *pin, io_t inc );

//--- data initialization stuff
// bytes in data file (may be zero)
extern u32 scaler_get_data_bytes(ParamType p);
// bytes in rep file (may be zero)
extern u32 scaler_get_rep_bytes(ParamType p);

// get pathname for data file (if any)
extern const char* scaler_get_data_path(ParamType p);
// get pathname for representation file (if any)
extern const char* scaler_get_rep_path(ParamType p);

// get offsets into NV memory
extern u32 scaler_get_data_offset(ParamType p);
extern u32 scaler_get_rep_offset(ParamType p);

// get pointers to NV memory for table assignment
extern const s32* scaler_get_nv_data(ParamType p);
extern const s32* scaler_get_nv_rep(ParamType p);

#endif
