/* op_ckdiv.

*/

// asf
#include "print_funcs.h"

// bees
#include "app_timers.h"
#include "net_poll.h"
#include "net_protected.h"
#include "op_ckdiv.h"

//-------------------------------------------------
//----- descriptor
static const char* op_ckdiv_instring	= "ENABLE\0 PERIOD\0 VAL\0    DIV\0    ";
static const char* op_ckdiv_outstring	= "TICK\0   TOCK\0   ";
static const char* op_ckdiv_opstring	= "CKDIV";

//-------------------------------------------------
//----- static function declaration
static void op_ckdiv_in_enable	(op_ckdiv_t* ckdiv, const io_t v);
static void op_ckdiv_in_period	(op_ckdiv_t* ckdiv, const io_t v);
static void op_ckdiv_in_value	(op_ckdiv_t* ckdiv, const io_t v);
static void op_ckdiv_in_divide	(op_ckdiv_t* ckdiv, const io_t v);

// array of input functions
static op_in_fn op_ckdiv_in_fn[4] = {
  (op_in_fn)&op_ckdiv_in_enable,
  (op_in_fn)&op_ckdiv_in_period,
  (op_in_fn)&op_ckdiv_in_value,
  (op_in_fn)&op_ckdiv_in_divide,
};


// pickles
static u8* op_ckdiv_pickle(op_ckdiv_t* ckdiv, u8* dst);
static const u8* op_ckdiv_unpickle(op_ckdiv_t* ckdiv, const u8* src);

// timer manipulation
static inline void op_ckdiv_set_timer(op_ckdiv_t* ckdiv);
static inline void op_ckdiv_unset_timer(op_ckdiv_t* ckdiv);

// polled-operator handler
void op_ckdiv_poll_handler(void* op);

//---------------------------------------------
//----- external function definition

/// initialize
void op_ckdiv_init(void* op) {
  op_ckdiv_t* ckdiv = (op_ckdiv_t*)op;
  // operator superclass
  ckdiv->super.numInputs = 4;
  ckdiv->super.numOutputs = 2;
  ckdiv->outs[0] = -1;
  ckdiv->outs[1] = -1;
  // polled operator superclass
  ckdiv->op_poll.handler = (poll_handler_t)(&op_ckdiv_poll_handler);
  ckdiv->op_poll.op = ckdiv;
  ckdiv->super.in_fn = op_ckdiv_in_fn;
  // input value array
  ckdiv->super.in_val = ckdiv->in_val;
  ckdiv->in_val[0] = &(ckdiv->enable);
  ckdiv->in_val[1] = &(ckdiv->period);
  ckdiv->in_val[2] = &(ckdiv->value);
  ckdiv->in_val[3] = &(ckdiv->divide);

  // pickles
  ckdiv->super.pickle = (op_pickle_fn)(&op_ckdiv_pickle);
  ckdiv->super.unpickle = (op_unpickle_fn)(&op_ckdiv_unpickle);
  // output array
  ckdiv->super.out = ckdiv->outs;
  // strings
  ckdiv->super.opString = op_ckdiv_opstring;
  ckdiv->super.inString = op_ckdiv_instring;
  ckdiv->super.outString = op_ckdiv_outstring;
  // type
  ckdiv->super.type = eOpCkdiv;
  /// state
  ckdiv->period = op_from_int(125);
  ckdiv->enable = 0;
  ckdiv->value = OP_ONE;
  ckdiv->divide = OP_ONE;
  ckdiv->cacheDivision = ckdiv->ticklength / ckdiv->divide;
  ckdiv->cacheRemainder = ckdiv->ticklength % ckdiv->divide;

  // timer (unlinked)
  ckdiv->timer.next = NULL;
}

void op_ckdiv_deinit(void* ckdiv) {
  op_ckdiv_unset_timer(ckdiv);
}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input enable / disable
void op_ckdiv_in_enable	(op_ckdiv_t* ckdiv, const io_t v) {
  //  print_dbg("\r\n op_ckdiv_in_enable: 0x");
  //  print_dbg_hex((u32)(v));

  if(v > 0) {
    if(ckdiv->enable == 0) {
      ckdiv->enable = OP_ONE;
      ckdiv->timer.ticks = ckdiv->cacheDivision;
      ckdiv->tockremainder = ckdiv->cacheRemainder;
      ckdiv->tocks = 0;
      op_ckdiv_set_timer(ckdiv);
      net_activate(ckdiv, 0, ckdiv->value);
    }
  } else {
    if(ckdiv->enable > 0) {
      ckdiv->enable = 0;
      op_ckdiv_unset_timer(ckdiv);
    }
  }
}

// input polling period
void op_ckdiv_in_period (op_ckdiv_t* ckdiv, const io_t v) {
  if((v) < 5) {
    ckdiv->period = 5;
  } else {
    ckdiv->period = v;
  }
  op_ckdiv_in_divide(ckdiv, ckdiv->divide);// re-check divide value
					   // doesn't overload network

  // XXX hack alert! the reported time (in milliseconds) seem to be
  // always off a bit.  Measured them & adding this correction

  ckdiv->ticklength = ckdiv->period;

  ckdiv->ticklength = ckdiv->ticklength << 1;
  ckdiv->ticklength -= ckdiv->ticklength >> 7;
  ckdiv->ticklength -= ckdiv->ticklength >> 8;
  ckdiv->ticklength -= ckdiv->ticklength >> 9;

  ckdiv->cacheDivision = ckdiv->ticklength / ckdiv->divide;
  ckdiv->cacheRemainder = ckdiv->ticklength % ckdiv->divide;

  ckdiv->timer.ticks = ckdiv->cacheDivision;
  ckdiv->tockremainder = ckdiv->cacheRemainder;
  ckdiv->tocks = 0;
  if(ckdiv->enable) {
    op_ckdiv_unset_timer(ckdiv);
    op_ckdiv_set_timer(ckdiv);
  }
  net_activate(ckdiv, 0, ckdiv->value);
}


// input value
void op_ckdiv_in_value (op_ckdiv_t* ckdiv, const io_t v) {
  ckdiv->value = v;
}

void op_ckdiv_in_divide (op_ckdiv_t* ckdiv, const io_t v) {
  if(v < 1) {
    ckdiv->divide = 1;
  } else if (v * 5 > ckdiv->period) {
    // do nothing, so the last effective value is retained
  }
  else {
    ckdiv->divide = v;
  }
  ckdiv->cacheDivision = ckdiv->ticklength / ckdiv->divide;
  ckdiv->cacheRemainder = ckdiv->ticklength % ckdiv->divide;
  ckdiv->tocks = 0;
  if(ckdiv->enable) {
    op_ckdiv_unset_timer(ckdiv);
    op_ckdiv_set_timer(ckdiv);
  }
  net_activate(ckdiv, 0, ckdiv->value);
}


// poll event handler
void op_ckdiv_poll_handler(void* op) {
  op_ckdiv_t* ckdiv = (op_ckdiv_t*)op;
  ckdiv->timer.ticks = ckdiv->cacheDivision;
  ckdiv->tockremainder += ckdiv->ticklength % ckdiv->divide;
  if(ckdiv->tockremainder >= ckdiv->divide) {
    ckdiv->tockremainder -= ckdiv->divide;
    ckdiv->timer.ticks += 1;
  }
  ckdiv->tocks += ckdiv->timer.ticks;
  if(ckdiv->tocks >= ckdiv->ticklength) {
    /* printf("tick\n"); */
    ckdiv->tockremainder = ckdiv->ticklength % ckdiv->divide;
    ckdiv->tocks = 0;
    net_activate(ckdiv, 0, ckdiv->value);
  } else {
    /* printf("tock\n"); */
    net_activate(ckdiv, 1, ckdiv->value);
  }
}


//===== pickles

u8* op_ckdiv_pickle(op_ckdiv_t* ckdiv, u8* dst) {
  dst = pickle_io(ckdiv->enable, dst);
  dst = pickle_io(ckdiv->period, dst);
  dst = pickle_io(ckdiv->value, dst);
  dst = pickle_io(ckdiv->divide, dst);
  return dst;
}

const u8* op_ckdiv_unpickle(op_ckdiv_t* ckdiv, const u8* src) {
  src = unpickle_io(src, &(ckdiv->enable));
  src = unpickle_io(src, &(ckdiv->period));
  src = unpickle_io(src, &(ckdiv->value));
  src = unpickle_io(src, &(ckdiv->divide));

  // this op has a bunch of state in non-input state variables, so
  // recrank the handles...
  op_ckdiv_in_period(ckdiv, ckdiv->period);
  op_ckdiv_in_divide(ckdiv, ckdiv->divide);
  op_ckdiv_enable(ckdiv, ckdiv->enable);
  return src;
}

// timer manipulation
static inline void op_ckdiv_set_timer(op_ckdiv_t* ckdiv) {
  //  timer_add(&(ckdiv->timer), op_to_int(ckdiv->period), &op_ckdiv_callback, (void*)ckdiv);
  /* timer_add(&(ckdiv->timer), op_to_int(ckdiv->period), &app_custom_event_callback, ckdiv); */
  timers_set_custom(&(ckdiv->timer), op_to_int(ckdiv->period), &(ckdiv->op_poll) );
  
  //  print_dbg("\r\n op_ckdiv add timer, return value: ");
  //  print_dbg(ret ? "1" : "0");
}

static inline void op_ckdiv_unset_timer(op_ckdiv_t* ckdiv) {
  timer_remove(&(ckdiv->timer));
  //  timers_unset_custom(&(ckdiv->timer));

  //  print_dbg("\r\n op_ckdiv remove timer, return value: ");
  //  print_dbg(ret ? "1" : "0");
}
