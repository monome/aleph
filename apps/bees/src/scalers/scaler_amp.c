// asf
#include "print_funcs.h"
// aleph-avr32
#include "memory.h"
// bees
#include "files.h"
#include "param_scaler.h"
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
  print_dbg("\r\n requesting amp_scaler value for input: 0x");
  print_dbg_hex((u32)in);

  return tabVal[in >> inRshift];
}

static s32 scaler_amp_rep(io_t in) {
  return tabRep[in >> inRshift];
}

//-----------------------
//---- ectern funcs
// init function
void scaler_amp_init(ParamScaler* sc, const ParamDesc* desc) {
  // check descriptor
  if( desc->type != eParamTypeAmp) {
    print_dbg("\r\n !!! warning: wrong param type for amp scaler");
  }
  
  // init flag for static data
  if(initFlag) { return; } else {
    initFlag = 1;
    // allocate
    print_dbg("\r\n allocating static memory for amp scalers");
    tabVal = (s32*)alloc_mem(tabSize * 4);
    tabRep = (s32*)alloc_mem(tabSize * 4);
    
    // load gain data
    print_dbg("\r\n loading gain scaler data from sdcard");
    files_load_scaler_name("scaler_amp_val.bin", tabVal, tabSize);
    files_load_scaler_name("scaler_amp_rep.bin", tabRep, tabSize);
  }

  /// FIXME: should consider requested param range,
  //  and compute a customized multiplier here if necessary.
  // for now, scaling functions are static.

  print_dbg("\r\n finished loading amp scaler data from files.");
  
  // assign lookup functions
  print_dbg("\r\n assigning lookup functions... 0x");
  print_dbg_hex((u32)&scaler_amp_val);
  print_dbg(", 0x");
  print_dbg_hex((u32)&scaler_amp_rep);

  print_dbg("\r\n scaler address: 0x");
  print_dbg_hex((u32)sc);
  print_dbg("\r\n scaler get_val address: 0x");
  print_dbg_hex((u32)&(sc->get_val));
  print_dbg("\r\n scaler get_rep address: 0x");
  print_dbg_hex((u32)&(sc->get_rep));

 
  //  sc->get_val = &scaler_amp_val;
  //  sc->get_rep = &scaler_amp_rep;
  
  /// god damn, why is this crashing??
  //   sc->get_val = NULL;
  //  sc->get_rep = NULL;
  print_dbg("\r\n (just kidding)");
  
  print_dbg(" \r\n done.");
  
  //// FIXME: add tuning functions (???)
  //  sc->tune = NULL;
  //  sc->numTune = 0;  
}
