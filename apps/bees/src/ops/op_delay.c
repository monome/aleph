/* op_delay.

*/

// asf
#include "print_funcs.h"

// bees
#include "app_timers.h"
#include "net_poll.h"
#include "net_protected.h"
#include "op_delay.h"

//-------------------------------------------------
//----- descriptor
static const char* op_delay_instring	= "VAL\0    TIME\0   CLEAR\0  ";
static const char* op_delay_outstring	= "VAL\0    ";
static const char* op_delay_opstring	= "DELAY";

//-------------------------------------------------
//----- static function declaration
static void op_delay_in_val	(op_delay_t* delay, const io_t v);
static void op_delay_in_time	(op_delay_t* delay, const io_t v);
static void op_delay_in_clear	(op_delay_t* delay, const io_t v);

// array of input functions
static op_in_fn op_delay_in_fn[3] = {
  (op_in_fn)&op_delay_in_val,
  (op_in_fn)&op_delay_in_time,
  (op_in_fn)&op_delay_in_clear,
};


// pickles
static u8* op_delay_pickle(op_delay_t* delay, u8* dst);
static const u8* op_delay_unpickle(op_delay_t* delay, const u8* src);

// timer manipulation
static inline void op_delay_set_timer(op_delay_t* delay);
static inline void op_delay_unset_timer(op_delay_t* delay);

// polled-operator handler
void op_delay_poll_handler(void* op);

//---------------------------------------------
//----- external function definition

/// initialize
void op_delay_init(void* op) {
  op_delay_t* delay = (op_delay_t*)op;
  // operator superclass
  delay->super.numInputs = 3;
  delay->super.numOutputs = 1;
  delay->outs[0] = -1;
  // polled operator superclass
  delay->op_poll.handler = (poll_handler_t)(&op_delay_poll_handler);
  delay->op_poll.op = delay;
  delay->super.in_fn = op_delay_in_fn;
  // input value array
  delay->super.in_val = delay->in_val;
  delay->in_val[0] = &(delay->val);
  delay->in_val[1] = &(delay->ms);
  delay->in_val[2] = &(delay->clear);

  // pickles
  delay->super.pickle = (op_pickle_fn)(&op_delay_pickle);
  delay->super.unpickle = (op_unpickle_fn)(&op_delay_unpickle);
  // output array
  delay->super.out = delay->outs;
  // strings
  delay->super.opString = op_delay_opstring;
  delay->super.inString = op_delay_instring;
  delay->super.outString = op_delay_outstring;
  // type
  delay->super.type = eOpDelay;
  /// state
  delay->ms = op_from_int(125);
  delay->clear = 0;
  delay->val = 0;
  // timer (unlinked)
  delay->timer.next = NULL;
}

void op_delay_deinit(void* delay) {
  op_delay_unset_timer(delay);
}

//-------------------------------------------------
//----- static function definition

//===== operator input

void op_delay_in_time (op_delay_t* delay, const io_t v) {
  if((v) < 1) {
    delay->ms = 1;
  } else {
    delay->ms = v;
  }
  delay->timer.ticks = op_to_int(delay->ms);
}


void op_delay_in_val (op_delay_t* delay, const io_t v) {
  delay->val = v;
  
  op_delay_unset_timer(delay);
  op_delay_set_timer(delay);
}

void op_delay_in_clear (op_delay_t* delay, const io_t v) {
  if(v) op_delay_unset_timer(delay);
}



// poll event handler
void op_delay_poll_handler(void* op) {
  op_delay_t* delay = (op_delay_t*)op;
  op_delay_unset_timer(delay);
  net_activate(delay->outs[0], delay->val, &(delay->super));
}


//===== pickles

u8* op_delay_pickle(op_delay_t* delay, u8* dst) {
  dst = pickle_io(delay->ms, dst);
  return dst;
}

const u8* op_delay_unpickle(op_delay_t* delay, const u8* src) {
  src = unpickle_io(src, &(delay->ms));
  return src;
}

// timer manipulation
static inline void op_delay_set_timer(op_delay_t* delay) {
  print_dbg("\r\n op_delay set timer, start ");
  timers_set_custom(&(delay->timer), op_to_int(delay->ms), &(delay->op_poll) );
  
  print_dbg("\r\n op_delay set timer, done");
}

static inline void op_delay_unset_timer(op_delay_t* delay) {
  print_dbg("\r\n op_delay_unset_timer, start ");
  timer_remove(&(delay->timer));
  print_dbg("\r\n op_delay_unset_timer, done ");
}
