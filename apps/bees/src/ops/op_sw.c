#include "net_protected.h"
#include "print_funcs.h"

#include "pickle.h"
#include "op_sw.h"

//-------------------------------------------------
//----- descriptor
static const char* op_sw_instring = "TOG     MUL     ";
static const char* op_sw_outstring = "VAL     ";
static const char* op_sw_opstring = "SW";

//-------------------------------------------------
//----- static function declaration


// UI increment
static void op_sw_inc(op_sw_t* sw, const s16 idx, const io_t inc);
// set inputs
//static void op_sw_in_state(op_sw_t* sw, const io_t v);
static void op_sw_in_tog(op_sw_t* sw, const io_t v);
static void op_sw_in_mul(op_sw_t* sw, const io_t );
// pickle / unpickle
static u8* op_sw_pickle(op_sw_t* sw, u8* dst);
static u8* op_sw_unpickle(op_sw_t* sw, const u8* src);

// array of input functions 
static op_in_fn op_sw_in[2] = {
  //  (op_in_fn)&op_sw_in_state,
  (op_in_fn)&op_sw_in_tog,
  (op_in_fn)&op_sw_in_mul
};

//----- external function definition

/// initialize
void op_sw_init(void* op) {
  op_sw_t* sw = (op_sw_t*)op;

  // superclass functions
  sw->super.inc_fn = (op_inc_fn)&op_sw_inc;
  sw->super.in_fn = op_sw_in;
  sw->super.pickle = (op_pickle_fn) (&op_sw_pickle);
  sw->super.unpickle = (op_unpickle_fn) (&op_sw_unpickle);
  
  // superclass state
  sw->super.numInputs = 2;
  sw->super.numOutputs = 1;
  sw->outs[0] = -1;
 
  sw->super.in_val = sw->in_val;
  //  sw->in_val[0] = &(sw->state);
  sw->in_val[0] = &(sw->tog);
  sw->in_val[1] = &(sw->mul);

  sw->super.out = sw->outs;
  sw->super.opString = op_sw_opstring;
  sw->super.inString = op_sw_instring;
  sw->super.outString = op_sw_outstring;
  sw->super.type = eOpSwitch;
  sw->super.flags |= (1 << eOpFlagSys);

  // class state
  sw->state = 0;
  sw->mul = OP_ONE;
  sw->tog = 0;
}

//-------------------------------------------------
//----- static function definition

//===== operator input

/* // input state */
/* static void op_sw_in_state(op_sw_t* sw, const io_t v) { */
/*   //  print_dbg("\r\n\r\n op_sw_in_state, current state: 0x"); */
/*   //  print_dbg_hex((u32)(sw->state)); */
/*   //  print_dbg(", input: 0x"); */
/*   //  print_dbg_hex((u32)(v)); */

/*   if (sw->tog) { */
/*     // toggle mode, sw state toggles on positive input */
/*     if ( (v) > 0) { */

/*       if ((sw->state) == 0) {  */
/* 	//	print_dbg("\r\n op_sw (toggle), state was == 0, setting to mul : 0x"); */
/* 	//	print_dbg_hex((u32)(sw->mul)); */
/* 	sw->state = sw->mul; */
/*       } else { */
/* 	//	print_dbg("\r\n op_sw (toggle), state was !=0, setting to 0 "); */
/* 	sw->state = 0;  */
/*       } */
/*       //      print_dbg("\r\n output: 0x"); */
/*       //      print_dbg_hex((u32)(sw->state)); */
    
/*       net_activate(sw->outs[0], sw->state, sw); */
/*     }  */
/*   } else { */
/*     // momentary mode, sw value takes input */
/*     //    print_dbg("\r\n op_sw (momentary), old state: 0x"); */
/*     //    print_dbg_hex((u32)(sw->state)); */

/*     if((v) > 0) { sw->state = sw->mul; } else { sw->state = 0; } */

/*     //    print_dbg(", new state: 0x"); */
/*     //    print_dbg_hex((u32)(sw->state)); */

/*     net_activate(sw->outs[0], sw->state, sw); */
/*   } */
/* } */

// input toggle mode
static void op_sw_in_tog(op_sw_t* sw, const io_t v) {
  //  print_dbg("\r\n op_sw_in_mul");
  if ((v) > 0) { sw->tog = OP_ONE; } else  { sw->tog = 0; } 
}

// input multiplier
static void op_sw_in_mul(op_sw_t* sw, const io_t v) {
  //  print_dbg("\r\n op_sw_in_mul");

  sw->mul = v;
  if (sw->state > 0) {
    sw->state = (v);
    net_activate(sw->outs[0], sw->state, sw);
  }
}

//===== UI input

// increment
static void op_sw_inc(op_sw_t* sw, const s16 idx, const io_t inc) {
  io_t val;
  //  print_dbg("\r\n op_sw_inc");
  switch(idx) {
  /* case 0: // current value */
  /*   op_sw_in_state(sw, inc); */
  /*   break; */
  case 0: // toggle mode
    op_sw_in_tog(sw, inc);
    break;
  case 1: // multiplier
    val = op_sadd(sw->mul, inc);
    op_sw_in_mul(sw, val);
    break;
  }
}


// serialization
u8* op_sw_pickle(op_sw_t* sw, u8* dst) {
  // store state variables
  dst = pickle_io(sw->state, dst);
  dst = pickle_io(sw->mul, dst);
  dst = pickle_io(sw->tog, dst);
  return dst;
}

u8* op_sw_unpickle(op_sw_t* sw, const u8* src) {
  // retreive state variables
  src = unpickle_io(src, (u32*)&(sw->state));
  src = unpickle_io(src, (u32*)&(sw->mul));
  src = unpickle_io(src, (u32*)&(sw->tog));
  return (u8*)src;
}


// handle input from system 
void op_sw_sys_input(op_sw_t* sw, u8 v) {
  if (sw->tog) {
    // toggle mode, sw state toggles on positive input
    if ( (v) > 0) {
      if ((sw->state) == 0) { 
	sw->state = sw->mul;
      } else {
	sw->state = 0; 
      }
      net_activate(sw->outs[0], sw->state, sw);
    } 
  } else {
    // momentary mode, sw value takes input
    if((v) > 0) { sw->state = sw->mul; } else { sw->state = 0; }
    net_activate(sw->outs[0], sw->state, sw);
  }
}
