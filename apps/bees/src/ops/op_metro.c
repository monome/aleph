/* op_metro.

*/

// asf
#include "print_funcs.h"

// bees
#include "app_timers.h"
#include "net_poll.h"
#include "net_protected.h"
#include "op_metro.h"

//-------------------------------------------------
//----- descriptor
static const char* op_metro_instring	= "ENABLE\0 PERIOD\0 VAL\0    ";
static const char* op_metro_outstring	= "TICK\0   ";
static const char* op_metro_opstring	= "METRO";

//-------------------------------------------------
//----- static function declaration
static void op_metro_in_enable	(op_metro_t* metro, const io_t v);
static void op_metro_in_period	(op_metro_t* metro, const io_t v);
static void op_metro_in_value	(op_metro_t* metro, const io_t v);

// array of input functions
static op_in_fn op_metro_in_fn[3] = {
  (op_in_fn)&op_metro_in_enable,
  (op_in_fn)&op_metro_in_period,
  (op_in_fn)&op_metro_in_value,
};


// pickles
static u8* op_metro_pickle(op_metro_t* metro, u8* dst);
static const u8* op_metro_unpickle(op_metro_t* metro, const u8* src);

// timer manipulation
static inline void op_metro_set_timer(op_metro_t* metro);
static inline void op_metro_unset_timer(op_metro_t* metro);

// polled-operator handler
void op_metro_poll_handler(void* op);

//---------------------------------------------
//----- external function definition

/// initialize
void op_metro_init(void* op) {
  op_metro_t* metro = (op_metro_t*)op;
  // operator superclass
  metro->super.numInputs = 3;
  metro->super.numOutputs = 1;
  metro->outs[0] = -1;
  // polled operator superclass
  metro->op_poll.handler = (poll_handler_t)(&op_metro_poll_handler);
  metro->op_poll.op = metro;
  metro->super.in_fn = op_metro_in_fn;
  // input value array
  metro->super.in_val = metro->in_val;
  metro->in_val[0] = &(metro->enable);
  metro->in_val[1] = &(metro->period);
  metro->in_val[2] = &(metro->value);

  // pickles
  metro->super.pickle = (op_pickle_fn)(&op_metro_pickle);
  metro->super.unpickle = (op_unpickle_fn)(&op_metro_unpickle);
  // output array
  metro->super.out = metro->outs;
  // strings
  metro->super.opString = op_metro_opstring;
  metro->super.inString = op_metro_instring;
  metro->super.outString = op_metro_outstring;
  // type
  metro->super.type = eOpMetro;
  /// state
  metro->period = op_from_int(125);
  metro->enable = 0;
  metro->value = OP_ONE;
  // timer (unlinked)
  metro->timer.next = NULL;
}

void op_metro_deinit(void* metro) {
  op_metro_unset_timer(metro);
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input enable / disable
void op_metro_in_enable	(op_metro_t* metro, const io_t v) {
  //  print_dbg("\r\n op_metro_in_enable: 0x");
  //  print_dbg_hex((u32)(v));

  if((v) > 0) {
    //    print_dbg(" (input value high) ");
    if(metro->enable == 0) {
      metro->enable = OP_ONE;
      op_metro_set_timer(metro);
      
    }
  } else {
    //    print_dbg(" (input value low) ");
    if(metro->enable > 0) {
      metro->enable = 0;
      op_metro_unset_timer(metro);
    }
  }
}

// input polling period
void op_metro_in_period (op_metro_t* metro, const io_t v) {
  if((v) < 5) {
    metro->period = 5;
  } else {
    metro->period = v;
  }
  metro->timer.ticks = op_to_int(metro->period);
}


// input value
void op_metro_in_value (op_metro_t* metro, const io_t v) {
  metro->value = v;
}


// poll event handler
void op_metro_poll_handler(void* op) {
  op_metro_t* metro = (op_metro_t*)op;
  //  print_dbg("\r\n op_metro timer callback, value: 0x");
  //  print_dbg_hex((u32)(metro->value));
  net_activate(metro->outs[0], metro->value, &(metro->super));
}


//===== pickles

u8* op_metro_pickle(op_metro_t* metro, u8* dst) {
  dst = pickle_io(metro->enable, dst);
  dst = pickle_io(metro->period, dst);
  dst = pickle_io(metro->value, dst);
  return dst;
}

const u8* op_metro_unpickle(op_metro_t* metro, const u8* src) {
  src = unpickle_io(src, &(metro->enable));
  src = unpickle_io(src, &(metro->period));
  src = unpickle_io(src, &(metro->value));
  if(metro->enable) {
    op_metro_set_timer(metro);
  }
  return src;
}

// timer manipulation
static inline void op_metro_set_timer(op_metro_t* metro) {
  //  timer_add(&(metro->timer), op_to_int(metro->period), &op_metro_callback, (void*)metro);
  /* timer_add(&(metro->timer), op_to_int(metro->period), &app_custom_event_callback, metro); */
  timers_set_custom(&(metro->timer), op_to_int(metro->period), &(metro->op_poll) );
  
  //  print_dbg("\r\n op_metro add timer, return value: ");
  //  print_dbg(ret ? "1" : "0");
}

static inline void op_metro_unset_timer(op_metro_t* metro) {
  timer_remove(&(metro->timer));
  //  timers_unset_custom(&(metro->timer));

  //  print_dbg("\r\n op_metro remove timer, return value: ");
  //  print_dbg(ret ? "1" : "0");
}
