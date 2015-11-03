/* ctl_op.c
 * aleph-avr32
 *
 * operator implmentation (base  class)
 */

// std
//#include <stdio.h>
// asf
//#ifdef ARCH_AVR32
#include "print_funcs.h"
//#endif
// aleph
#include "net.h"
#include "op.h"
#include "op_math.h"
#include "op_derived.h"
#include "param.h"
#include "play.h"

// user-creatable operator types
// this order is arbitrary, no need to synchronize with class enum
const op_id_t userOpTypes[NUM_USER_OP_TYPES] = {
  eOpAccum,
  eOpAdd,
  eOpMonomeArc, // "arc"
  eOpBars,
  eOpBars8,
  eOpBignum,
  eOpBits,
  eOpChange,
  eOpDelay,
  eOpDiv,
  eOpDivr,
  eOpFade,
  eOpGate,
  eOpMonomeGridRaw, // "grid"
  eOpHid,
  eOpHistory,
  eOpIs,
  eOpLife,
  eOpList2,
  eOpList8,
  eOpList16,
  eOpLogic,
  eOpMetro,
  eOpCascades, // "mp"
  eOpMidiCC,
  eOpMidiNote,
  eOpMidiOutCC,
  eOpMidiOutNote,
  eOpMod,
  eOpMul,
  eOpRandom,
  eOpRoute,
  eOpRoute8,
  eOpRoute16,
  eOpScreen,
  eOpSerial,
  eOpShl,
  eOpShr,
  eOpSplit, // "Y" , but only in connection graph
  eOpSplit4, // "Y4"
  eOpStep,
  eOpSub,
  eOpThresh,
  eOpTimer,
  eOpTog,
  eOpWW
};


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
  }, {
    .name = "MUL",
    .size = sizeof(op_mul_t) ,
    .init = &op_mul_init,
    .deinit = NULL
  }, {
    .name = "GATE",
    .size = sizeof(op_gate_t), 
    .init = &op_gate_init,
    .deinit = NULL
  }, {
    .name = "GRID",
    .size = sizeof(op_mgrid_raw_t),
    .init = &op_mgrid_raw_init,
    .deinit = &op_mgrid_raw_deinit
  }, {
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
    .deinit = &op_life_deinit
  }, {
    .name = "HISTORY",
    .size = sizeof(op_history_t),
    .init = &op_history_init,
    .deinit = NULL    
  }, {
    .name = "BIGNUM",
    .size = sizeof(op_bignum_t),
    .init = &op_bignum_init,
    .deinit = &op_bignum_deinit
  }, {
    .name = "SCREEN",
    .size = sizeof(op_screen_t),
    .init = &op_screen_init,
    .deinit = &op_screen_deinit
  }, {
    .name = "SPLIT4",
    .size = sizeof(op_split4_t),
    .init = &op_split4_init,
    .deinit = NULL
  }, {
    .name = "DELAY",
    .size = sizeof(op_delay_t),
    .init = &op_delay_init,
    .deinit = NULL
  }, {
    .name = "ROUTE",
    .size = sizeof(op_route_t),
    .init = &op_route_init,
    .deinit = NULL
  }, {
    .name = "MIDICC",
    .size = sizeof(op_midi_cc_t),
    .init = &op_midi_cc_init,
    .deinit = op_midi_cc_deinit
  }, {
    .name = "MOUT_NOTE",
    .size = sizeof(op_midi_out_note_t),
    .init = &op_midi_out_note_init,
    .deinit = NULL
  }, {
    .name = "LIST16",
    .size = sizeof(op_list16_t),
    .init = &op_list16_init,
    .deinit = NULL
  }, {
    .name = "STEP",
    .size = sizeof(op_step_t),
    .init = &op_step_init,
    .deinit = &op_step_deinit   
  }, {
    .name = "ROUTE8",
    .size = sizeof(op_route8_t),
    .init = &op_route8_init,
    .deinit = NULL
  }, {
    .name = "MP",   // CASCADES
    .size = sizeof(op_cascades_t),
    .init = &op_cascades_init,
    .deinit = &op_cascades_deinit   
  }, {
    .name = "BARS",
    .size = sizeof(op_bars_t),
    .init = &op_bars_init,
    .deinit = &op_bars_deinit   
  }, {
    .name = "SERIAL",
    .size = sizeof(op_serial_t),
    .init = &op_serial_init,
    .deinit = &op_serial_deinit   
  }, {
    .name = "HID",
    .size = sizeof(op_hid_word_t),
    .init = &op_hid_word_init,
    .deinit = &op_hid_word_deinit   
  }, {
    .name = "WW",
    .size = sizeof(op_ww_t),
    .init = &op_ww_init,
    .deinit = &op_ww_deinit   
  }, {
    .name = "ARC",
    .size = sizeof(op_marc_t),
    .init = &op_marc_init,
    .deinit = &op_marc_deinit
  }, {
    .name = "FADE",
    .size = sizeof(op_fade_t),
    .init = &op_fade_init,
    .deinit = NULL
  }, {
    .name = "DIVR",
    .size = sizeof(op_divr_t),
    .init = &op_divr_init,
    .deinit = NULL
  }, {
    .name = "SHL",
    .size = sizeof(op_shl_t),
    .init = &op_shl_init,
    .deinit = NULL
  }, {
    .name = "SHR",
    .size = sizeof(op_shr_t),
    .init = &op_shr_init,
    .deinit = NULL
  }, {
    .name = "CHANGE",
    .size = sizeof(op_change_t),
    .init = &op_change_init,
    .deinit = NULL
  }, {
    .name = "ROUTE16",
    .size = sizeof(op_route16_t),
    .init = &op_route16_init,
    .deinit = NULL
  }, {
    .name = "BARS8",
    .size = sizeof(op_bars8_t),
    .init = &op_bars8_init,
    .deinit = &op_bars8_deinit   
  }, {
    .name = "MOUT_CC",
    .size = sizeof(op_midi_out_cc_t),
    .init = &op_midi_out_cc_init,
    .deinit = NULL
  },
};


// input and output strings are all the same length, concatenated
// lazy
static const u8 inStringChars = 8;
static const u8 outStringChars = 8;

//===============================================
//===  base class definittions

// initialize operator at memory
s16 op_init(op_t* op, op_id_t opId) {
  //  int i;
  // no flags by default
  op->flags = 0x00000000;
  // set function pointers to NULL
  op->pickle = NULL;
  op->unpickle = NULL;
  // run class init function
  (*(op_registry[opId].init))(op);
  return 0;
}

// this is the order of operators as presented to the user 

// de-initialize operator
s16 op_deinit(op_t* op) {
  op_class_deinit f = op_registry[op->type].deinit;
  if(f != NULL) {
    /* print_dbg("\r\n de-initializing operator at address 0x"); */
    /* print_dbg_hex((u32)op); */
    (*f)(op);
  }
  return 0;
}



const char* op_in_name(op_t* op, const u8 idx) {
  /// names now have spaces zapped to nulls.
  // so we can safely return direct pointer to name string
  return (op->inString + (inStringChars * idx));  
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
  (*(op->in_fn[idx]))(op, val);  
}

// increment input value
void op_inc_in_val(op_t* op, const s16 idx, const io_t inc) {
  /* print_dbg("\r\n op_inc_in_val, "); */
  /* print_dbg(" op @ 0x"); */
  /* print_dbg_hex((u32)op); */
  /* print_dbg(" old : 0x"); */
  /* print_dbg_hex((u32)op_get_in_val(op, idx)); */
  /* print_dbg(" inc : 0x"); */
  /* print_dbg_hex((u32)inc); */
  /* print_dbg(" new : 0x"); */
  /* print_dbg_hex( (u32)op_sadd( op_get_in_val(op, idx), inc) ); */    
  op_set_in_val( op, idx, op_sadd( op_get_in_val(op, idx), inc) );
}
