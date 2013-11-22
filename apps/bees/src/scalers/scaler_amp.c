// asf
#include "print_funcs.h"
// aleph-avr32
#include "memory.h"
// bees
#include "files.h"
#include "scaler.h"
#include "scaler_amp.h"

// table size
static const u8 tabBits = 10;
static const u32 tabSize = 1024;
// shift from io_t size
static const u8 inRshift = 6;

static s32* tabVal;
static s32* tabRep;

static u8 initFlag = 0;

//-------------------
//--- static funcs
static s32 scaler_amp_val(io_t in) {
  return tabVal[in >> inRshift];
}

static s32 scaler_amp_rep(io_t in) {
  return tabRep[in >> inRshift];
}

//-----------------------
//---- ectern funcs
// init function
// this should only be called once
void scaler_amp_init(scaler* sc, const ParamDesc* desc) {
  // only allow one initialzation
  if(initFlag) { return; } else { initFlag = 1; }
  // check descriptor
  if( desc->type != eParamTypeAmp) {
    print_dbg("\r\n !!! warning: wrong param type for amp scaler");
  }


  // allocate
  tabVal = (s32*)alloc_mem(tabSize * 4);
  tabRep = (s32*)alloc_mem(tabSize * 4);
  // load gain data
  files_load_scaler_name("scaler_amp_val.bin", tabVal, tabSize);
  files_load_scaler_name("scaler_amp_rep.bin", tabRep, tabSize);

  /// FIXME: should consider requested param range,
  //  and rescale here if necessary

  // assign lookup functions
  sc->get_val = &(scaler_amp_val);
  sc->get_rep = &(scaler_amp_rep);  
  
  //// FIXME: add tuning functions (???)
  sc->numTune = 0;  
}
