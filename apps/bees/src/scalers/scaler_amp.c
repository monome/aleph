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


//-----------------------
//---- extern funcs

s32 scaler_amp_val(io_t in) {
  print_dbg("\r\n requesting amp_scaler value for input: 0x");
  print_dbg_hex((u32)in);

  return tabVal[in >> inRshift];
}

s32 scaler_amp_rep(io_t in) {
  print_dbg("\r\n requesting amp_scaler representation for input: 0x");
  print_dbg_hex((u32)in);

  print_dbg(" ; value: 0x");
  print_dbg_hex(tabRep[in >> inRshift]);

  return tabRep[in >> inRshift];
}

// init function
void scaler_amp_init(ParamScaler* sc, const ParamDesc* desc) {
  // check descriptor
  if( desc->type != eParamTypeAmp) {
    print_dbg("\r\n !!! warning: wrong param type for amp scaler");
  }
  
  // init flag for static data
  if(initFlag) { 
    ;;
  } else {
    initFlag = 1;
    // allocate
    print_dbg("\r\n allocating static memory for amp scalers");
    tabVal = (s32*)alloc_mem(tabSize * 4);
    tabRep = (s32*)alloc_mem(tabSize * 4);
    
    // load gain data
    print_dbg("\r\n loading gain scaler data from sdcard");
    files_load_scaler_name("scaler_amp_val.bin", tabVal, tabSize);
    files_load_scaler_name("scaler_amp_rep.bin", tabRep, tabSize);
   print_dbg("\r\n finished loading amp scaler data from files.");
  }

  sc->type = eParamTypeAmp;

  /// FIXME: should consider requested param range,
  //  and compute a customized multiplier here if necessary.
  // for now, scaling functions are static.>.????

  ///// this was driving me nuts so instead used a static table of FPs in param_scaler.c 

  /* // assign lookup functions */
  /* print_dbg("\r\n assigning lookup functions... 0x"); */
  /* print_dbg_hex((u32)&scaler_amp_val); */
  /* print_dbg(", 0x"); */
  /* print_dbg_hex((u32)&scaler_amp_rep); */

  /* print_dbg("\r\n scaler address: 0x"); */
  /* print_dbg_hex((u32)sc); */
  /* print_dbg("\r\n scaler get_val address: 0x"); */
  /* print_dbg_hex((u32)&(sc->get_val)); */
  /* print_dbg("\r\n scaler get_rep address: 0x"); */
  /* print_dbg_hex((u32)&(sc->get_rep)); */

  /* //  sc->get_val = &scaler_amp_val; */
  /* //  sc->get_rep = &scaler_amp_rep; */
 
  /* /// god damn, why is this crashing?? */
  /* //   sc->get_val = (scaler_get_value_fn)NULL; */
  /* // sc->get_rep = (scaler_get_rep_fn)NULL; */
  /* //  print_dbg("\r\n (just kidding)"); */


  /* /// if we don't cast + dereference, this access crashes! */
  /* /// wtf !??!??! */
  /* print_dbg("\r\n scaler get_val value: 0x"); */
  /* print_dbg_hex(*((const u32*)&(sc->get_val))); */
  /* print_dbg("\r\n scaler get_rep value: 0x"); */
  /* print_dbg_hex(*((const u32*)&(sc->get_rep))); */
 

  /* /// ok, an utterly barbaric attempt...  */
  /* /// why does it work? */
  /* *((u32*)&(sc->get_val)) = (u32)&(scaler_amp_val); */
  /* *((u32*)&(sc->get_rep)) = (u32)&(scaler_amp_rep); */
  
  /* print_dbg(" \r\n done."); */
  
 


  //// FIXME: add tuning functions (???)
  //  sc->tune = NULL;
  //  sc->numTune = 0;  
}
