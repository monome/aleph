/* ctl_op.c
 * aleph-avr32
 *
 * operator implmentation (base  class)
 */

// std
//#include <stdio.h>
// asf
#ifdef ARCH_AVR32
#include "print_funcs.h"
#endif
// aleph
#include "net.h"
#include "op.h"
#include "op_math.h"
#include "op_derived.h"
#include "param.h"
#include "play.h"

// operator class registry
////// WARNING:
// must be laid out identically to eOpId enum in op.h!
// therefore, recommend adding in chronological order (?)
const op_desc_t op_registry[numOpClasses] = {
  {
    .name = "SW",
    .size = sizeof(op_sw_t), 
    .init = &op_sw_init, 
    .deinit = NULL
  }, {
    .name = "ENC", 
    .size = sizeof(op_enc_t),
    .init = &op_enc_init,
    .deinit = NULL
  }, {
    .name = "ADD",
    .size = sizeof(op_add_t) ,
    .init = &op_add_init,
    .deinit = NULL
  } , {
    .name = "MUL",
    .size = sizeof(op_mul_t) ,
    .init = &op_mul_init,
    .deinit = NULL
  } , {
    .name = "GATE",
    .size = sizeof(op_gate_t), 
    .init = &op_gate_init,
    .deinit = NULL
  } , {
    .name = "GRID",
    .size = sizeof(op_mgrid_raw_t),
    .init = &op_mgrid_raw_init,
    .deinit = &op_mgrid_raw_deinit
  } , {
    .name = "MIDINOTE",
    .size = sizeof(op_midi_note_t),
    .init = &op_midi_note_init,
    .deinit = &op_midi_note_deinit
  }, {
    .name = "ADC",
    .size = sizeof(op_adc_t),
    .init = &op_adc_init,
    .deinit = &op_adc_deinit
  }, {
    .name = "METRO",
    .size = sizeof(op_metro_t),
    .init = &op_metro_init,
    .deinit = &op_metro_deinit
  }, {
    .name = "PRESET",
    .size = sizeof(op_preset_t),
    .init = &op_preset_init,
    .deinit = NULL
  }, {
    .name = "TOG",
    .size = sizeof(op_tog_t),
    .init = &op_tog_init,
    .deinit = NULL
  }, {
    .name = "ACCUM",
    .size = sizeof(op_accum_t),
    .init = &op_accum_init,
    .deinit = NULL
  }, {
    .name = "SPLIT",
    .size = sizeof(op_split_t),
    .init = &op_split_init,
    .deinit = NULL
  }, {
    .name = "DIV",
    .size = sizeof(op_div_t),
    .init = &op_div_init,
    .deinit = NULL
  }, {
    .name = "SUB",
    .size = sizeof(op_sub_t),
    .init = &op_sub_init,
    .deinit = NULL
  }, {
    .name = "TIMER",
    .size = sizeof(op_timer_t),
    .init = &op_timer_init,
    .deinit = NULL
  }, {
    .name = "RANDOM",
    .size = sizeof(op_random_t),
    .init = &op_random_init,
    .deinit = NULL    
  }, {
    .name = "LIST8",
    .size = sizeof(op_list8_t),
    .init = &op_list8_init,
    .deinit = NULL    
  }, {
    .name = "THRESH",
    .size = sizeof(op_thresh_t),
    .init = &op_thresh_init,
    .deinit = NULL    
  }, {
    .name = "MOD",
    .size = sizeof(op_mod_t),
    .init = &op_mod_init,
    .deinit = NULL    
  }, {
    .name = "BITS",
    .size = sizeof(op_bits_t),
    .init = &op_bits_init,
    .deinit = NULL    
  }, {
    .name = "IS",
    .size = sizeof(op_is_t),
    .init = &op_is_init,
    .deinit = NULL    
  }, {
    .name = "LOGIC",
    .size = sizeof(op_logic_t),
    .init = &op_logic_init,
    .deinit = NULL    
  }, {
    .name = "LIST2",
    .size = sizeof(op_list2_t),
    .init = &op_list2_init,
    .deinit = NULL    
  }, {
    .name = "LIFE",
    .size = sizeof(op_life_t),
    .init = &op_life_init,
    .deinit = NULL    
  }, {
    .name = "HISTORY",
    .size = sizeof(op_history_t),
    .init = &op_history_init,
    .deinit = NULL    
  }

};


// input and output strings are all the same length, concatenated
// lazy
static const u8 inStringChars = 8;
static const u8 outStringChars = 8;

//===============================================
//===  base class definittions

// initialize operator at memory
s16 op_init(op_t* op, op_id_t opId) {
  // no flags by default
  op->flags = 0x00000000;
  // set function pointers to NULL
  op->pickle = NULL;
  op->unpickle = NULL;
  // run class init function
  (*(op_registry[opId].init))(op);
  return 0;
}

// de-initialize operator
s16 op_deinit(op_t* op) {
  op_class_deinit f = op_registry[op->type].deinit;
  
  if(f != NULL) {
    print_dbg("\r\n de-initializing operator at address 0x");
    print_dbg_hex((u32)op);
    (*f)(op);
  }
  return 0;
}



const char* op_in_name(op_t* op, const u8 idx) {
  static char str[16];
  u8 i;
  // str = (op->inString + (inStringChars * idx));
  for(i=0; i<inStringChars; i++) {
    str[i] = *(op->inString + (inStringChars * idx) + i);
  }
  str[inStringChars] = '\0';
  return str;
}

const char* op_out_name(op_t* op, const u8 idx) {
  static char str[16];
  u8 i;
  for(i=0; i<outStringChars; i++) {
    str[i] = *(op->outString + (outStringChars * idx) + i);
  }
  str[outStringChars] = '\0';
  return str;
}

// get input value
io_t op_get_in_val(op_t* op, s16 idx) {
  return *(op->in_val[idx]);
}

// set input value
void op_set_in_val(op_t* op, s16 idx, io_t val) {
  //  volatile io_t * const pIn = (op->in_val[idx]);
  //  *pIn = val;
  //  (*(op->in_fn[idx]))(op, pIn);  

  //  print_dbg("\r\n setting op input, value: 0x");
  //  print_dbg_hex(val);

  (*(op->in_fn[idx]))(op, val);  

  // TODO: check for play flag and stuff.. ?
  // or, do this in activation
  //  play_input(idx);
}

/// err... user is calling subclass directly, whatever

// pickle
/* u8* op_pickle(op_t* op, u8* dst) { */
/*   return (*(op->pickle))(op, dst); */
/* } */

// unpickle
/* const u8* op_unpickle(op_t* op, const u8* src) { */
/*     return (*(op->unpickle))(op, src); */
/* } */
