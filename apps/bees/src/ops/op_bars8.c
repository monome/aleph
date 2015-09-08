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
#include "op_bars8.h"


//-------------------------------------------------
//----- static vars

//-- descriptor
static const char* op_bars8_instring = "ENABLE\0 PERIOD\0 MODE\0   A\0      B\0      C\0      D\0      E\0      F\0      G\0      H\0      ";
static const char* op_bars8_outstring = "";
static const char* op_bars8_opstring = "BARS8";

//-------------------------------------------------
//----- static function declaration

// set inputs
static void op_bars8_in_enable(op_bars8_t* screen, const io_t v);
static void op_bars8_in_period(op_bars8_t* screen, const io_t v);
static void op_bars8_in_mode(op_bars8_t* screen, const io_t x);
static void op_bars8_in_a(op_bars8_t* screen, const io_t y);
static void op_bars8_in_b(op_bars8_t* screen, const io_t y);
static void op_bars8_in_c(op_bars8_t* screen, const io_t y);
static void op_bars8_in_d(op_bars8_t* screen, const io_t y);
static void op_bars8_in_e(op_bars8_t* screen, const io_t y);
static void op_bars8_in_f(op_bars8_t* screen, const io_t y);
static void op_bars8_in_g(op_bars8_t* screen, const io_t y);
static void op_bars8_in_h(op_bars8_t* screen, const io_t y);

// pickle / unpickle
static u8* op_bars8_pickle(op_bars8_t* screen, u8* dst);
static u8* op_bars8_unpickle(op_bars8_t* screen, const u8* src);

//// redraw
static void op_bars8_redraw(op_bars8_t* screen);

// timer manipulation
static inline void op_bars8_set_timer(op_bars8_t* screen);
static inline void op_bars8_unset_timer(op_bars8_t* screen);

// polled-operator handler
void op_bars8_poll_handler(void* op);

// array of input functions 
static op_in_fn op_bars8_in[11] = {
  (op_in_fn)&op_bars8_in_enable,
  (op_in_fn)&op_bars8_in_period,
  (op_in_fn)&op_bars8_in_mode,
  (op_in_fn)&op_bars8_in_a,
  (op_in_fn)&op_bars8_in_b,
  (op_in_fn)&op_bars8_in_c,
  (op_in_fn)&op_bars8_in_d,
  (op_in_fn)&op_bars8_in_e,
  (op_in_fn)&op_bars8_in_f,
  (op_in_fn)&op_bars8_in_g,
  (op_in_fn)&op_bars8_in_h
};

//----- external function definition

/// initialize
void op_bars8_init(void* op) {
  op_bars8_t* bars8 = (op_bars8_t*)op;

  // superclass functions
  bars8->super.in_fn = op_bars8_in;
  bars8->super.pickle = (op_pickle_fn) (&op_bars8_pickle);
  bars8->super.unpickle = (op_unpickle_fn) (&op_bars8_unpickle);
    // polled operator superclass
  bars8->op_poll.handler = (poll_handler_t)(&op_bars8_poll_handler);
  bars8->op_poll.op = bars8;

  // superclass val
  bars8->super.numInputs = 11;
  bars8->super.numOutputs = 0;
 
  bars8->super.in_val = bars8->in_val;
  bars8->in_val[0] = &(bars8->enable);
  bars8->in_val[1] = &(bars8->period);
  bars8->in_val[2] = &(bars8->mode);
  bars8->in_val[3] = &(bars8->a);
  bars8->in_val[4] = &(bars8->b);
  bars8->in_val[5] = &(bars8->c);
  bars8->in_val[6] = &(bars8->d);
  bars8->in_val[7] = &(bars8->e);
  bars8->in_val[8] = &(bars8->f);
  bars8->in_val[9] = &(bars8->g);
  bars8->in_val[10] = &(bars8->h);

  bars8->super.out = bars8->outs;
  bars8->super.opString = op_bars8_opstring;
  bars8->super.inString = op_bars8_instring;
  bars8->super.outString = op_bars8_outstring;
  bars8->super.type = eOpBars8;

  // class val
  bars8->enable = 0;
  bars8->period = op_from_int(100);
  bars8->mode = 0;
  bars8->a = 0;
  bars8->b = 0;
  bars8->c = 0;
  bars8->d = 0;
  bars8->e = 0;
  bars8->f = 0;
  bars8->g = 0;
  bars8->h = 0;

  // init graphics
  /*.. this is sort of retarded, 
  doing stuff normally accomplished in region_alloc() or in static init,
  but doing a dumb memory hack on it to share a tmp data space between instances.
  */  
  bars8->reg.dirty = 0;
  bars8->reg.x = 0;
  bars8->reg.y = 0;
  bars8->reg.w = OP_BARS8_PX_W;
  bars8->reg.h = OP_BARS8_PX_H;
  bars8->reg.len = OP_BARS8_GFX_BYTES;
  bars8->reg.data = (u8*) (bars8->regData);

  region_fill(&(bars8->reg), 0);
}


/// de-initialize
void op_bars8_deinit(void* op) {
  op_bars8_t* bars8 = (op_bars8_t*)op;
  if(bars8->enable > 0) {
    op_gfx_disable();
    op_bars8_unset_timer(bars8);
  }
}

//-------------------------------------------------
//----- static function definition

//===== operator input
void op_bars8_in_enable(op_bars8_t* bars8, const io_t v  ) {
  if(v > 0) {
    if(bars8->enable > 0) {
      ;;
    } else {
      op_gfx_enable();
      bars8->enable = 1;
      op_bars8_set_timer(bars8);
      op_bars8_redraw(bars8);
    }
  } else { 
    if(bars8->enable > 0) {
      op_gfx_disable();
      bars8->enable = 0;
      op_bars8_unset_timer(bars8);
    } else {
      ;;
    }
  }
}

// input period
void op_bars8_in_period(op_bars8_t* bars8, const io_t v) {
  if((v) < 5) {
    bars8->period = 5;
  } else {
    bars8->period = v;
  }
  bars8->timer.ticks = op_to_int(bars8->period);
}

// input val
void op_bars8_in_mode(op_bars8_t* bars8, const io_t v) {
  ;;
}

void op_bars8_in_a(op_bars8_t* bars8, const io_t v) {
  if(v < 0) { bars8->a = 0; }
  else if(v > 128) { bars8->a = 128; }
  else { bars8->a = v; }
  op_bars8_redraw(bars8);
}

void op_bars8_in_b(op_bars8_t* bars8, const io_t v) {
  if(v < 0) { bars8->b = 0; }
  else if(v > 128) { bars8->b = 128; }
  else { bars8->b = v; }
  op_bars8_redraw(bars8);
}

void op_bars8_in_c(op_bars8_t* bars8, const io_t v) {
  if(v < 0) { bars8->c = 0; }
  else if(v > 128) { bars8->c = 128; }
  else { bars8->c = v; }
  op_bars8_redraw(bars8);
}

void op_bars8_in_d(op_bars8_t* bars8, const io_t v) {
  if(v < 0) { bars8->d = 0; }
  else if(v > 128) { bars8->d = 128; }
  else { bars8->d = v; }
  op_bars8_redraw(bars8);
}

void op_bars8_in_e(op_bars8_t* bars8, const io_t v) {
  if(v < 0) { bars8->e = 0; }
  else if(v > 128) { bars8->e = 128; }
  else { bars8->e = v; }
  op_bars8_redraw(bars8);
}

void op_bars8_in_f(op_bars8_t* bars8, const io_t v) {
  if(v < 0) { bars8->f = 0; }
  else if(v > 128) { bars8->f = 128; }
  else { bars8->f = v; }
  op_bars8_redraw(bars8);
}

void op_bars8_in_g(op_bars8_t* bars8, const io_t v) {
  if(v < 0) { bars8->g = 0; }
  else if(v > 128) { bars8->g = 128; }
  else { bars8->g = v; }
  op_bars8_redraw(bars8);
}

void op_bars8_in_h(op_bars8_t* bars8, const io_t v) {
  if(v < 0) { bars8->h = 0; }
  else if(v > 128) { bars8->h = 128; }
  else { bars8->h = v; }
  op_bars8_redraw(bars8);
}

// poll event handler
void op_bars8_poll_handler(void* op) {
  op_bars8_t* bars8 = (op_bars8_t*)op;
  region* r = &(bars8->reg);
  if(pageIdx == ePagePlay) {
    if(r->dirty) {
      screen_draw_region(0, 0, r->w, r->h, r->data);
      r->dirty = 0;
    }
  }
  else r->dirty = 1;
}


// serialization
u8* op_bars8_pickle(op_bars8_t* bars8, u8* dst) {
  // store val variables
  dst = pickle_io(bars8->enable, dst);
  dst = pickle_io(bars8->period, dst);
  dst = pickle_io(bars8->mode, dst);
  dst = pickle_io(bars8->a, dst);
  dst = pickle_io(bars8->b, dst);
  dst = pickle_io(bars8->c, dst);
  dst = pickle_io(bars8->d, dst);
  dst = pickle_io(bars8->e, dst);
  dst = pickle_io(bars8->f, dst);
  dst = pickle_io(bars8->g, dst);
  dst = pickle_io(bars8->h, dst);
  return dst;
}

u8* op_bars8_unpickle(op_bars8_t* bars8, const u8* src) {
  // retreive val  variables
  src = unpickle_io(src, (u32*)&(bars8->enable));
  src = unpickle_io(src, (u32*)&(bars8->period));
  src = unpickle_io(src, (u32*)&(bars8->mode));
  src = unpickle_io(src, (u32*)&(bars8->a));
  src = unpickle_io(src, (u32*)&(bars8->b));
  src = unpickle_io(src, (u32*)&(bars8->c));
  src = unpickle_io(src, (u32*)&(bars8->d));
  src = unpickle_io(src, (u32*)&(bars8->e));
  src = unpickle_io(src, (u32*)&(bars8->f));
  src = unpickle_io(src, (u32*)&(bars8->g));
  src = unpickle_io(src, (u32*)&(bars8->h));

  if(bars8->enable == 1) {
      op_bars8_redraw(bars8);
      op_gfx_enable();
      op_bars8_set_timer(bars8);
  }
   return (u8*)src;
}

// redraw with current state
void op_bars8_redraw(op_bars8_t* bars8) {
  if(bars8->enable <= 0) { return; }

  u8 i;

  // draw a pixel to the region, ha
  //*(bars8->reg.data + (bars8->y * op_bars8_PX_W) + bars8->x) = bars8->val;

  for(i=0;i<128;i++) {
    *(bars8->reg.data + (4096 + i)) = (bars8->a > i) * 14 + 1;
    *(bars8->reg.data + (4352 + i)) = (bars8->b > i) * 14 + 1;
    *(bars8->reg.data + (4608 + i)) = (bars8->c > i) * 14 + 1;
    *(bars8->reg.data + (4864 + i)) = (bars8->d > i) * 14 + 1;
    *(bars8->reg.data + (5376 + i)) = (bars8->e > i) * 14 + 1;
    *(bars8->reg.data + (5632 + i)) = (bars8->f > i) * 14 + 1;
    *(bars8->reg.data + (5888 + i)) = (bars8->g > i) * 14 + 1;
    *(bars8->reg.data + (6144 + i)) = (bars8->h > i) * 14 + 1;
  }

  bars8->reg.dirty = 1; 
}


// timer manipulation
static inline void op_bars8_set_timer(op_bars8_t* bars8) {
  timers_set_custom(&(bars8->timer), op_to_int(bars8->period), &(bars8->op_poll) );
}

static inline void op_bars8_unset_timer(op_bars8_t* bars8) {
  timer_remove(&(bars8->timer));
}
