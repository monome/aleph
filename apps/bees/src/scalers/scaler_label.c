// std
#include <string.h>
// asf
#include "print_funcs.h"
// aleph-avr32
#include "memory.h"
// bees
#include "files.h"
#include "param_scaler.h"
#include "scaler_label.h"

//-------------------
//--- static funcs

//-----------------------
//---- extern funcs

s32 scaler_labels_val(void* scaler, io_t in) {
  return (io_t) in;
}

char scaler_labels[MAX_LABELS][LABEL_LENGTH] = {"EFFECT", "ADC0", "ADC1", "ADC2", "ADC3", "ADC4", "GRAIN1", "GRAIN2", "PTOSC1", "PTOSC2", "ENV1", "ENV2", "LFO", "PING", "CV1", "CV2"};
int scaler_n_labels = 16;

static char *labelsWriteHead;
void scaler_start_parse_labels (void) {
  scaler_n_labels = 1;
  labelsWriteHead = &scaler_labels[0][0];
  *labelsWriteHead = 0;
}

void scaler_parse_labels_char (char c) {
  /* print_dbg("\r\nchar: "); */
  /* print_dbg_char(c); */
  /* print_dbg("(n="); */
  /* print_dbg_ulong(c); */
  /* print_dbg(")"); */
  if (scaler_n_labels >= MAX_LABELS) {
    print_dbg("\r\n tried to insert too many labels");
  }
  if (labelsWriteHead - &scaler_labels[scaler_n_labels][0] >= LABEL_LENGTH - 1) {
    *labelsWriteHead = 0;
    print_dbg("\r\n tried to insert over-length label");
  }
  if(c == 0) {
    print_dbg("\r\n asked to parse label char 0, file already ended");
    return;
  }
  if(c == '\n') {
    /* print_dbg("\r\nfinished parsing patchpoint: "); */
    *labelsWriteHead = 0;
    /* print_dbg(scaler_labels[scaler_n_labels-1]); */
    labelsWriteHead = &scaler_labels[scaler_n_labels][0];
    *labelsWriteHead = 0;
    scaler_n_labels++;
  } else if(c == 255) {
    *labelsWriteHead = 0;
  }else {
    *labelsWriteHead = c;
    labelsWriteHead++;
    *labelsWriteHead = 0;
  }
}

// init function
void scaler_labels_init(void* scaler) {
  /* print_dbg("\r\n entered scaler_patch_init"); */
  ParamScaler* sc = (ParamScaler*)scaler;

  /* print_dbg("\r\n initializing patchmatrix, scaler for param, label: "); */
  /* print_dbg(sc->desc->label); */

  sc->inMin = sc->desc->min;
  sc->inMax = sc->desc->max;
  /* scaler_n_labels = 16; */
  /* int i; */
  /* for(i=0; i < scaler_n_labels; i++) { */
  /*   strcpy(scaler_labels[i], scaler_labels_dummy[i]); */
  /* } */
  /* print_dbg("\r\n done initialising patchmatrix..."); */
  /* print_dbg("\r\n exiting scaler_patch_init"); */
}

void scaler_labels_str(char* dst, void* scaler,  io_t in) {
  ParamScaler* sc = (ParamScaler*)scaler;
  /* print_dbg("\r\n entered scaler_patch_str"); */
  //  u16 uin = BIT_ABS_16((s16)in) >> inRshift;
  if(in > sc->inMax) {
    in = sc->inMax;
  }
  if(in < sc->inMin) {
    in = sc->inMin;
  }
  strcpy(dst,scaler_labels[in]);

  /* print_dbg("\r\n exiting scaler_patch_str"); */
}

// get input given DSP value (use sparingly)
io_t scaler_labels_in(void* scaler, s32 x) {
  /* print_dbg("\r\n entered scaler_labels_in"); */
  ParamScaler* sc = (ParamScaler*)scaler;
  if(x < sc->inMin) { x = sc->inMin; }
  if(x > sc->inMax) { x = sc->inMax; }
  /* print_dbg("\r\n exiting scaler_labels_in"); */
  return (io_t) x;
}


// increment input by pointer, return value
s32 scaler_labels_inc(void* scaler, io_t* pin, io_t inc ) {
  /* print_dbg("\r\n entered scaler_patch_inc"); */
  /* print_dbg("\r\n *pin = "); */
  /* print_dbg_ulong(*pin); */
  /* print_dbg(", inc = "); */
  /* print_dbg_ulong(inc); */
  ParamScaler* sc = (ParamScaler*)scaler;
  // use saturation
  *pin = op_sadd(*pin, inc);
  /* print_dbg("\r\nafter satadd: "); */
  /* print_dbg_ulong(*pin); */

  if(*pin < sc->inMin) { *pin = sc->inMin; }
  if(*pin > sc->inMax) { *pin = sc->inMax; }

  /* print_dbg("\r\nafter clamp: "); */
  /* print_dbg_ulong(*pin); */
  // FIXME: no customization of output range.
  /* print_dbg("\r\n exiting scaler_patch_inc"); */
  return scaler_labels_val(sc, *pin);
}
