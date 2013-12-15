/* param_scaler.c
   bees

   parameter-scaling module.

*/


#include "print_funcs.h"

#include "param_scaler.h"
#include "types.h"

// array of data bytes required per param type
static u32 scalerDataBytes[eParamNumTypes] = {
  0, 	//  eParamTypeBool,
  0, 	//  eParamTypeFix,
  1024, 	//  eParamTypeAmp,
  1024, 	//  eParamTypeIntegrator,
  1024, 	//  eParamTypeNote,
  1024, 	//  eParamTypeSvfFreq,
};

// array of representation bytes required per param type
static u32 scalerRepBytes[eParamNumTypes] = {
  0, 	//  eParamTypeBool,
  0, 	//  eParamTypeFix,
  1024, 	//  eParamTypeAmp,
  1024, 	//  eParamTypeIntegrator,
  0, 	//  eParamTypeNote,
  0, 	//  eParamTypeSvfFreq,
};

// array of data paths per param type
static const char scalerDataPath[eParamNumTypes][32] = {
  "", 	//  eParamTypeBool,
  "", 	//  eParamTypeFix,
  "scaler_amp_val.dat", 	//  eParamTypeAmp,
  "scaler_integrator_val.dat", 	//  eParamTypeIntegrator,
  "scaler_note_val.dat", 	//  eParamTypeNote,
  "scaler_svf_fc_val.dat", 	//  eParamTypeSvfFreq,
};

// array of representation paths per param type
static const char scalerRepPath[eParamNumTypes][32] = {
  "", 	//  eParamTypeBool,
  "", 	//  eParamTypeFix,
  "scaler_amp_rep.dat", 	//  eParamTypeAmp,
  "scaler_integrator_rep.dat", 	//  eParamTypeIntegrator,
  "", 	//  eParamTypeNote,
  "", 	//  eParamTypeSvfFreq,
};

// pretty wack, sorry:
// arrays of offsets into scaler NV data, for value and rep
static const u32 scalerDataOffset[eParamNumTypes] = {
    0, 	//  eParamTypeBool,
    0, 	//  eParamTypeFix,
    0, 	//  eParamTypeAmp,
    1024, 	//  eParamTypeIntegrator,
    2048, 	//  eParamTypeNote,
    3072, 	//  eParamTypeSvfFreq,
};
// put rep data after value data, just easier to check visually
static const u32 scalerRepOffset[eParamNumTypes] = {
  0, 	//  eParamTypeBool,
  0, 	//  eParamTypeFix,
  4096, 	//  eParamTypeAmp,
  5120, 	//  eParamTypeIntegrator,
  0, 	//  eParamTypeNote,
  0, 	//  eParamTypeSvfFreq,
};

// array of pointers to initialization functions.
// order must be in sync with types enum in param_common.h
scaler_init_fn scaler_init_pr[eParamNumTypes] = {
  &scaler_bool_init,
  &scaler_fix_init,
  &scaler_amp_init,
  &scaler_integrator_init,
  &scaler_note_init,
  &scaler_svf_fc_init,
};

/// FIXME: 
/// doing this rather improperly as a workaround,
/// was having super weird problems in class init? not getting it.
// so for now, a static array of fn pointers.
// their orders must all be in sync with types enum in param_common.h
// (sorry...)

// array of pointers to get_value functions.
scaler_get_value_fn scaler_get_val_pr[eParamNumTypes] = {
  &scaler_bool_val,
  &scaler_fix_val,
  &scaler_amp_val,
  &scaler_integrator_val,
  &scaler_note_val,
  &scaler_svf_fc_val,
};

// array of pointers to get_str functions.
scaler_get_str_fn scaler_get_str_pr[eParamNumTypes] = {
  &scaler_bool_str,
  &scaler_fix_str,
  &scaler_amp_str,
  &scaler_integrator_str,
  &scaler_note_str,
  &scaler_svf_fc_str,
};


// array of pointers to get_in functions.
scaler_get_in_fn scaler_get_in_pr[eParamNumTypes] = {
  &scaler_bool_in,
  &scaler_fix_in,
  &scaler_amp_in,
  &scaler_integrator_in,
  &scaler_note_in,
  &scaler_svf_fc_in,
};

// array of pointers to inc functions.
scaler_inc_fn scaler_inc_pr[eParamNumTypes] = {
  &scaler_bool_inc,
  &scaler_fix_inc,
  &scaler_amp_inc,
  &scaler_integrator_inc,
  &scaler_note_inc,
  &scaler_svf_fc_inc,
};


//-------------------------------,
//---- extern function
void scaler_init(ParamScaler* sc, const ParamDesc* desc) {
  // store pointer to constant descriptor data
  sc->desc = desc;
  if(scaler_init_pr[desc->type] != NULL) {
    (*(scaler_init_pr[desc->type]))(sc);
  }
}

// get DSP value given input
s32 scaler_get_value(ParamScaler* sc, io_t in) {
  print_dbg("\r\n scaler_get_value, type: ");
  print_dbg_ulong(sc->desc->type);

  scaler_get_value_fn fn = scaler_get_val_pr[sc->desc->type];
  if(fn != NULL) {
    return (*fn)(sc, in);
  } else {
    print_dbg("\r\n null function pointer in scaler_get_value");
    return 0;
  }
}

// get UI string representation for input
void scaler_get_str(char* dst, ParamScaler* sc, io_t in) {
  scaler_get_str_fn fn = scaler_get_str_pr[sc->desc->type];
  if(fn != NULL) {
    (*fn)(dst, sc, in);
  }

}

// get input given DSP value (use sparingly)
io_t scaler_get_in(ParamScaler* sc, s32 value) {
  scaler_get_in_fn fn =  scaler_get_in_pr[sc->desc->type];
  if( fn != NULL) {
    return (*fn)(sc, value);
  } else {
    return 0;
  }
}

// increment input
extern s32 scaler_inc(ParamScaler* sc, io_t * pin, io_t inc ) {
  scaler_inc_fn fn =  scaler_inc_pr[sc->desc->type];
  if( fn != NULL) {
    return (*fn)(sc, pin, inc);
  } else {
    return 0;
  }
}


// bytes in data file (may be zero)
u32 scaler_get_data_bytes(ParamType p) {
  return scalerDataBytes[p];
}

// bytes in rep file (may be zero)
u32 scaler_get_rep_bytes(ParamType p) {
  return scalerRepBytes[p];
}

//--- data initializatoin stuff
// get pathname for data file (if any)
const char* scaler_get_data_path(ParamType p) {
  return scalerDataPath[p];
}

// get pathname for representation file (if any)
const char* scaler_get_rep_path(ParamType p) { 
  return scalerRepPath[p];
}

// get offsets into NV memory
u32 scaler_get_data_offset(ParamType p) {
    return scalerDataOffset[p];
}

u32 scaler_get_rep_offset(ParamType p) {
  return scalerRepOffset[p];
}
