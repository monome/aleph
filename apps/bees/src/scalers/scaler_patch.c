// std
#include <string.h>
// asf
#include "print_funcs.h"
// aleph-avr32
#include "memory.h"
// bees
#include "files.h"
#include "param_scaler.h"
#include "scaler_patch.h"

//-------------------
//--- static funcs

//-----------------------
//---- extern funcs

s32 scaler_patch_val(void* scaler, io_t in) {
  return (io_t) in;
}

char scaler_patch_patchPoints[MAX_PATCHPOINTS][PATCHPOINT_LENGTH] = {"EFFECT", "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "GRAIN1", "GRAIN2", "PTOSC1", "PTOSC2", "ENV1", "ENV2", "LFO", "PING", "CV1", "CV2"};
int scaler_n_patchPoints = 16;

static char *patchPointWriteHead;
void scaler_start_parse_patchnames (void) {
  scaler_n_patchPoints = 1;
  patchPointWriteHead = &scaler_patch_patchPoints[0][0];
  *patchPointWriteHead = 0;
}

void scaler_parse_patchname_char (char c) {
  print_dbg("\r\nchar: ");
  print_dbg_char(c);
  print_dbg("(n=");
  print_dbg_ulong(c);
  print_dbg(")");
  if (scaler_n_patchPoints >= MAX_PATCHPOINTS) {
    print_dbg("\r\n tried to insert too many patchpoints");
  }
  if (patchPointWriteHead - &scaler_patch_patchPoints[scaler_n_patchPoints][0] >= PATCHPOINT_LENGTH - 1) {
    *patchPointWriteHead = 0;
    print_dbg("\r\n tried to insert over-length patchpoint");
  }
  if(c == 0) {
    print_dbg("\r\n asked to parse patchname char 0, file already ended");
    return;
  }
  if(c == '\n') {
    print_dbg("\r\nfinished parsing patchpoint: ");
    *patchPointWriteHead = 0;
    print_dbg(scaler_patch_patchPoints[scaler_n_patchPoints-1]);
    patchPointWriteHead = &scaler_patch_patchPoints[scaler_n_patchPoints][0];
    *patchPointWriteHead = 0;
    scaler_n_patchPoints++;
  } else {
    *patchPointWriteHead = c;
    patchPointWriteHead++;
  }
}

// init function
void scaler_patch_init(void* scaler) {
  print_dbg("\r\n entered scaler_patch_init");
  ParamScaler* sc = (ParamScaler*)scaler;

  /* print_dbg("\r\n initializing patchmatrix, scaler for param, label: "); */
  /* print_dbg(sc->desc->label); */

  sc->inMin = sc->desc->min;
  sc->inMax = sc->desc->max;
  /* scaler_n_patchPoints = 16; */
  /* int i; */
  /* for(i=0; i < scaler_n_patchPoints; i++) { */
  /*   strcpy(scaler_patch_patchPoints[i], scaler_patch_patchPoints_dummy[i]); */
  /* } */
  /* print_dbg("\r\n done initialising patchmatrix..."); */
  print_dbg("\r\n exiting scaler_patch_init");
}

void scaler_patch_str(char* dst, void* scaler,  io_t in) {
  ParamScaler* sc = (ParamScaler*)scaler;
  print_dbg("\r\n entered scaler_patch_str");
  //  u16 uin = BIT_ABS_16((s16)in) >> inRshift;
  if(in > sc->inMax) {
    in = sc->inMax;
  }
  if(in < sc->inMin) {
    in = sc->inMin;
  }
  strcpy(dst,scaler_patch_patchPoints[in]);

  print_dbg("\r\n exiting scaler_patch_str");
}

// get input given DSP value (use sparingly)
io_t scaler_patch_in(void* scaler, s32 x) {
  print_dbg("\r\n entered scaler_patch_in");
  ParamScaler* sc = (ParamScaler*)scaler;
  if(x < sc->inMin) { x = sc->inMin; }
  if(x > sc->inMax) { x = sc->inMax; }
  print_dbg("\r\n exiting scaler_patch_in");
  return (io_t) x;
}


// increment input by pointer, return value
s32 scaler_patch_inc(void* scaler, io_t* pin, io_t inc ) {
  print_dbg("\r\n entered scaler_patch_inc");
  print_dbg("\r\n *pin = ");
  print_dbg_ulong(*pin);
  print_dbg(", inc = ");
  print_dbg_ulong(inc);
  ParamScaler* sc = (ParamScaler*)scaler;
  // use saturation
  *pin = op_sadd(*pin, inc);
  print_dbg("\r\nafter satadd: ");
  print_dbg_ulong(*pin);

  if(*pin < sc->inMin) { *pin = sc->inMin; }
  if(*pin > sc->inMax) { *pin = sc->inMax; }

  print_dbg("\r\nafter clamp: ");
  print_dbg_ulong(*pin);
  // FIXME: no customization of output range.
  print_dbg("\r\n exiting scaler_patch_inc");
  return scaler_patch_val(sc, *pin);
}
