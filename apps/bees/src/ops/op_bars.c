// avr32-lib
#include "region.h"
#include "screen.h"

// bees
#include "app_timers.h"
#include "net_poll.h"
#include "net_protected.h"
#include "op_gfx.h"
#include "pages.h"
#include "print_funcs.h"
#include "pickle.h"
#include "op_bars.h"


//-------------------------------------------------
//----- static vars

//-- descriptor
static const char* op_bars_instring = "ENABLE\0 PERIOD\0 MODE\0   A\0      B\0      C\0      D\0      ";
static const char* op_bars_outstring = "";
static const char* op_bars_opstring = "BARS";

//-------------------------------------------------
//----- static function declaration

// set inputs
static void op_bars_in_enable(op_bars_t* screen, const io_t v);
static void op_bars_in_period(op_bars_t* screen, const io_t v);
static void op_bars_in_mode(op_bars_t* screen, const io_t x);
static void op_bars_in_a(op_bars_t* screen, const io_t y);
static void op_bars_in_b(op_bars_t* screen, const io_t y);
static void op_bars_in_c(op_bars_t* screen, const io_t y);
static void op_bars_in_d(op_bars_t* screen, const io_t y);

// pickle / unpickle
static u8* op_bars_pickle(op_bars_t* screen, u8* dst);
static u8* op_bars_unpickle(op_bars_t* screen, const u8* src);

//// redraw
static void op_bars_redraw(op_bars_t* screen);

// timer manipulation
static inline void op_bars_set_timer(op_bars_t* screen);
static inline void op_bars_unset_timer(op_bars_t* screen);

// polled-operator handler
void op_bars_poll_handler(void* op);

// array of input functions 
static op_in_fn op_bars_in[7] = {
  (op_in_fn)&op_bars_in_enable,
  (op_in_fn)&op_bars_in_period,
  (op_in_fn)&op_bars_in_mode,
  (op_in_fn)&op_bars_in_a,
  (op_in_fn)&op_bars_in_b,
  (op_in_fn)&op_bars_in_c,
  (op_in_fn)&op_bars_in_d
};

//----- external function definition

/// initialize
void op_bars_init(void* op) {
  op_bars_t* bars = (op_bars_t*)op;

  // superclass functions
  bars->super.in_fn = op_bars_in;
  bars->super.pickle = (op_pickle_fn) (&op_bars_pickle);
  bars->super.unpickle = (op_unpickle_fn) (&op_bars_unpickle);
    // polled operator superclass
  bars->op_poll.handler = (poll_handler_t)(&op_bars_poll_handler);
  bars->op_poll.op = bars;

  // superclass val
  bars->super.numInputs = 7;
  bars->super.numOutputs = 0;
 
  bars->super.in_val = bars->in_val;
  bars->in_val[0] = &(bars->enable);
  bars->in_val[1] = &(bars->period);
  bars->in_val[2] = &(bars->mode);
  bars->in_val[3] = &(bars->a);
  bars->in_val[4] = &(bars->b);
  bars->in_val[5] = &(bars->c);
  bars->in_val[6] = &(bars->d);

  bars->super.out = bars->outs;
  bars->super.opString = op_bars_opstring;
  bars->super.inString = op_bars_instring;
  bars->super.outString = op_bars_outstring;
  bars->super.type = eOpBars;

  // class val
  bars->enable = 0;
  bars->period = op_from_int(100);
  bars->mode = 0;
  bars->a = 0;
  bars->b = 0;
  bars->c = 0;
  bars->d = 0;

  // init graphics
  /*.. this is sort of retarded, 
  doing stuff normally accomplished in region_alloc() or in static init,
  but doing a dumb memory hack on it to share a tmp data space between instances.
  */  
  bars->reg.dirty = 0;
  bars->reg.x = 0;
  bars->reg.y = 0;
  bars->reg.w = OP_BARS_PX_W;
  bars->reg.h = OP_BARS_PX_H;
  bars->reg.len = OP_BARS_GFX_BYTES;
  bars->reg.data = (u8*) (bars->regData);

  region_fill(&(bars->reg), 0);
}


/// de-initialize
void op_bars_deinit(void* op) {
  op_bars_t* bars = (op_bars_t*)op;
  if(bars->enable > 0) {
    op_gfx_disable();
    op_bars_unset_timer(bars);
  }
}

//-------------------------------------------------
//----- static function definition

//===== operator input
void op_bars_in_enable(op_bars_t* bars, const io_t v  ) {
  if(v > 0) {
    if(bars->enable > 0) {
      ;;
    } else {
      op_gfx_enable();
      bars->enable = 1;
      op_bars_set_timer(bars);
      op_bars_redraw(bars);
    }
  } else { 
    if(bars->enable > 0) {
      op_gfx_disable();
      bars->enable = 0;
      op_bars_unset_timer(bars);
    } else {
      ;;
    }
  }
}

// input period
void op_bars_in_period(op_bars_t* bars, const io_t v) {
  if((v) < 5) {
    bars->period = 5;
  } else {
    bars->period = v;
  }
  bars->timer.ticks = op_to_int(bars->period);
}

// input val
void op_bars_in_mode(op_bars_t* bars, const io_t v) {
  ;;
}

void op_bars_in_a(op_bars_t* bars, const io_t v) {
  if(v < 0) { bars->a = 0; }
  else if(v > 128) { bars->a = 128; }
  else { bars->a = v; }
  op_bars_redraw(bars);
}

void op_bars_in_b(op_bars_t* bars, const io_t v) {
  if(v < 0) { bars->b = 0; }
  else if(v > 128) { bars->b = 128; }
  else { bars->b = v; }
  op_bars_redraw(bars);
}

void op_bars_in_c(op_bars_t* bars, const io_t v) {
  if(v < 0) { bars->c = 0; }
  else if(v > 128) { bars->c = 128; }
  else { bars->c = v; }
  op_bars_redraw(bars);
}

void op_bars_in_d(op_bars_t* bars, const io_t v) {
  if(v < 0) { bars->d = 0; }
  else if(v > 128) { bars->d = 128; }
  else { bars->d = v; }
  op_bars_redraw(bars);
}



// poll event handler
void op_bars_poll_handler(void* op) {
  op_bars_t* bars = (op_bars_t*)op;
  region* r = &(bars->reg);
  if(pageIdx == ePagePlay) {
    if(r->dirty) {
      screen_draw_region(0, 0, r->w, r->h, r->data);
      r->dirty = 0;
    }
  }
  else r->dirty = 1;
}


// serialization
u8* op_bars_pickle(op_bars_t* bars, u8* dst) {
  // store val variables
  dst = pickle_io(bars->enable, dst);
  dst = pickle_io(bars->period, dst);
  dst = pickle_io(bars->mode, dst);
  dst = pickle_io(bars->a, dst);
  dst = pickle_io(bars->b, dst);
  dst = pickle_io(bars->c, dst);
  dst = pickle_io(bars->d, dst);
  return dst;
}

u8* op_bars_unpickle(op_bars_t* bars, const u8* src) {
  // retreive val  variables
  src = unpickle_io(src, (u32*)&(bars->enable));
  src = unpickle_io(src, (u32*)&(bars->period));
  src = unpickle_io(src, (u32*)&(bars->mode));
  src = unpickle_io(src, (u32*)&(bars->a));
  src = unpickle_io(src, (u32*)&(bars->b));
  src = unpickle_io(src, (u32*)&(bars->c));
  src = unpickle_io(src, (u32*)&(bars->d));

  if(bars->enable == 1) {
      op_bars_redraw(bars);
      op_gfx_enable();
      op_bars_set_timer(bars);
  }
  return (u8*)src;
}

// redraw with current state
void op_bars_redraw(op_bars_t* bars) {
  if(bars->enable <= 0) { return; }

  u8 i;

  // draw a pixel to the region, ha
  //*(bars->reg.data + (bars->y * op_bars_PX_W) + bars->x) = bars->val;

  for(i=0;i<128;i++) {
    *(bars->reg.data + (4352 + i)) = (bars->a > i) * 14 + 1;
    *(bars->reg.data + (4864 + i)) = (bars->b > i) * 14 + 1;
    *(bars->reg.data + (5376 + i)) = (bars->c > i) * 14 + 1;
    *(bars->reg.data + (5888 + i)) = (bars->d > i) * 14 + 1;
  }

  bars->reg.dirty = 1; 
}


// timer manipulation
static inline void op_bars_set_timer(op_bars_t* bars) {
  timers_set_custom(&(bars->timer), op_to_int(bars->period), &(bars->op_poll) );
}

static inline void op_bars_unset_timer(op_bars_t* bars) {
  timer_remove(&(bars->timer));
}
