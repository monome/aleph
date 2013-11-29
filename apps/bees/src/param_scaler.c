/* param_scaler.c
   bees

   parameter-scaling module.

*/


#include "print_funcs.h"

#include "param_scaler.h"
#include "types.h"

// type-specific includes
#include "scalers/scaler_amp.h"

// array of pointers to initialization functions.
scaler_init_fn scaler_init_pr[eParamNumTypes] = {
  &scaler_amp_init,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

/// doing this rather improperly as a workaround,
/// super weird problems in class init? not getting it.
// array of pointers to get_value functions.
scaler_get_value_fn scaler_get_val_pr[eParamNumTypes] = {
  &scaler_amp_val,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};

// array of pointers to get_str functions.
scaler_get_str_fn scaler_get_str_pr[eParamNumTypes] = {
  &scaler_amp_str,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};


// array of pointers to get_in functions.
scaler_get_str_fn scaler_get_in_pr[eParamNumTypes] = {
  &scaler_amp_in,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};


//-------------------------------,
//---- extern function
void scaler_init(ParamScaler* sc, const ParamDesc* desc) {
  print_dbg("\r\n initializing scaler, type: ");
  print_dbg_ulong(desc->type);
  if(scaler_init_pr[desc->type] != NULL) {
    (*(scaler_init_pr[desc->type]))(sc, desc);
  }
}

s32 scaler_get_value(ParamScaler* sc, io_t in) {
  /* print_dbg("\r\n requesting scaler value for input: 0x"); */
  /* print_dbg_hex((u32)in); */

  /* print_dbg("\r\n scaler address: 0x"); */
  /* print_dbg_hex((u32)sc); */
  
  /* print_dbg("\r\n scaler type: "); */
  /* print_dbg_ulong(sc->type); */

  /* print_dbg("\r\n input value: "); */
  /* print_dbg_hex((u32)in); */

  //// FIXME: inefficient
  if(scaler_get_val_pr[sc->type] != NULL) {
    return (*(scaler_get_val_pr[sc->type]))(in);
  } else {
    print_dbg("\r\n null function pointer");
    return 0;
  }
}

// get UI string representation for input
void scaler_get_str(char* dst, ParamScaler* sc, io_t in) {

  //// FIXME: inefficient
  if(scaler_get_str_pr[sc->type] != NULL) {
    (*(scaler_get_str_pr[sc->type]))(dst, in);
  }
}

// get input given DSP value (use sparingly)
io_t scaler_get_in(ParamScaler* sc, s32 value) {
  if(scaler_get_str_pr[sc->type] != NULL) {
    (*(scaler_get_str_pr[sc->type]))(dst, in);
  }
}
