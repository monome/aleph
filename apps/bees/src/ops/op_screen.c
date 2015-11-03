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
#include "op_screen.h"


//-------------------------------------------------
//----- static vars

//-- descriptor
static const char* op_screen_instring = "ENABLE\0 PERIOD\0 VAL\0    FILL\0   X\0      Y\0      ";
static const char* op_screen_outstring = "";
static const char* op_screen_opstring = "SCREEN";

//-------------------------------------------------
//----- static function declaration

// set inputs
static void op_screen_in_enable(op_screen_t* screen, const io_t v);
static void op_screen_in_period(op_screen_t* screen, const io_t v);
static void op_screen_in_val(op_screen_t* screen, const io_t v);
static void op_screen_in_fill(op_screen_t* screen, const io_t v);
static void op_screen_in_x(op_screen_t* screen, const io_t x);
static void op_screen_in_y(op_screen_t* screen, const io_t y);

// pickle / unpickle
static u8* op_screen_pickle(op_screen_t* screen, u8* dst);
static u8* op_screen_unpickle(op_screen_t* screen, const u8* src);

//// redraw
static void op_screen_redraw(op_screen_t* screen);

// timer manipulation
static inline void op_screen_set_timer(op_screen_t* screen);
static inline void op_screen_unset_timer(op_screen_t* screen);

// polled-operator handler
void op_screen_poll_handler(void* op);

// array of input functions 
static op_in_fn op_screen_in[6] = {
  (op_in_fn)&op_screen_in_enable,
  (op_in_fn)&op_screen_in_period,
  (op_in_fn)&op_screen_in_val,
  (op_in_fn)&op_screen_in_fill,
  (op_in_fn)&op_screen_in_x,
  (op_in_fn)&op_screen_in_y,
};

//----- external function definition

/// initialize
void op_screen_init(void* op) {
  op_screen_t* screen = (op_screen_t*)op;

  // superclass functions
  screen->super.in_fn = op_screen_in;
  screen->super.pickle = (op_pickle_fn) (&op_screen_pickle);
  screen->super.unpickle = (op_unpickle_fn) (&op_screen_unpickle);
    // polled operator superclass
  screen->op_poll.handler = (poll_handler_t)(&op_screen_poll_handler);
  screen->op_poll.op = screen;

  // superclass val
  screen->super.numInputs = 6;
  screen->super.numOutputs = 0;
 
  screen->super.in_val = screen->in_val;
  screen->in_val[0] = &(screen->enable);
  screen->in_val[1] = &(screen->period);
  screen->in_val[2] = &(screen->val);
  screen->in_val[3] = &(screen->fill);
  screen->in_val[4] = &(screen->x);
  screen->in_val[5] = &(screen->y);

  screen->super.out = screen->outs;
  screen->super.opString = op_screen_opstring;
  screen->super.inString = op_screen_instring;
  screen->super.outString = op_screen_outstring;
  screen->super.type = eOpScreen;

  // class val
  screen->enable = 0;
  screen->period = op_from_int(50);
  screen->val = 0xf;
  screen->fill = 0;
  screen->x = 0;
  screen->y = 0;

  // init graphics
  /*.. this is sort of retarded, 
  doing stuff normally accomplished in region_alloc() or in static init,
  but doing a dumb memory hack on it to share a tmp data space between instances.
  */  
  screen->reg.dirty = 0;
  screen->reg.x = 0;
  screen->reg.y = 0;
  screen->reg.w = OP_SCREEN_PX_W;
  screen->reg.h = OP_SCREEN_PX_H;
  screen->reg.len = OP_SCREEN_GFX_BYTES;
  screen->reg.data = (u8*) (screen->regData);

  region_fill(&(screen->reg), 0);
}


/// de-initialize
void op_screen_deinit(void* op) {
  op_screen_t* screen = (op_screen_t*)op;
  if(screen->enable > 0) {
    op_gfx_disable();
    op_screen_unset_timer(screen);
  }
}

//-------------------------------------------------
//----- static function definition

//===== operator input
void op_screen_in_enable(op_screen_t* screen, const io_t v  ) {
  if(v > 0) {
    if(screen->enable > 0) {
      ;;
    } else {
      op_gfx_enable();
      screen->enable = 1;
      op_screen_set_timer(screen);
      op_screen_redraw(screen);
    }
  } else { 
    if(screen->enable > 0) {
      op_gfx_disable();
      screen->enable = 0;
      op_screen_unset_timer(screen);
    } else {
      ;;
    }
  }
}

// input period
void op_screen_in_period(op_screen_t* screen, const io_t v) {
  if((v) < 5) {
    screen->period = 5;
  } else {
    screen->period = v;
  }
  screen->timer.ticks = op_to_int(screen->period);
}

// input val
void op_screen_in_val(op_screen_t* screen, const io_t v) {
  if(v < 0) { screen->val = 0; }
  else if(v > 0xf) { screen->val = 0xf; }
  else { screen->val = v; }
  op_screen_redraw(screen);
}

// input fill
void op_screen_in_fill(op_screen_t* screen, const io_t v) {
  if(v < 0) { screen->fill = 0; }
  else if(v > 0xf) { screen->fill = 0xf; }
  else { screen->fill = v; }
  region_fill(&(screen->reg), v);
}

// input x position
void op_screen_in_x(op_screen_t* screen, const io_t v) {
  if (v > OP_SCREEN_X_MAX) {
    screen->x = screen->reg.x = OP_SCREEN_X_MAX; 
  } else if (v < 0) {
    screen->x = screen->reg.x = 0;
  } else {	 	
    screen->x = screen->reg.x = v;
  }
  op_screen_redraw(screen);
}

// input y position
void op_screen_in_y(op_screen_t* screen, const io_t v) {
  if (v > OP_SCREEN_Y_MAX) {
    screen->y = screen->reg.y = OP_SCREEN_Y_MAX;
  } else if (v < 0) {
    screen->y = screen->reg.y = 0;
  } else {   	
    screen->y = screen->reg.y = v;
  }
  op_screen_redraw(screen);
}

// poll event handler
void op_screen_poll_handler(void* op) {
  op_screen_t* screen = (op_screen_t*)op;
  region* r = &(screen->reg);
  if(pageIdx == ePagePlay) {
    if(r->dirty) {
      screen_draw_region(0, 0, r->w, r->h, r->data);
      r->dirty = 0;
    }
  }
  else r->dirty = 1;
}


// serialization
u8* op_screen_pickle(op_screen_t* screen, u8* dst) {
  // store val variables
  dst = pickle_io(screen->enable, dst);
  dst = pickle_io(screen->period, dst);
  dst = pickle_io(screen->val, dst);
  dst = pickle_io(screen->fill, dst);
  dst = pickle_io(screen->x, dst);
  dst = pickle_io(screen->y, dst);
  return dst;
}

u8* op_screen_unpickle(op_screen_t* screen, const u8* src) {
  // retreive val  variables
  src = unpickle_io(src, (u32*)&(screen->enable));
  src = unpickle_io(src, (u32*)&(screen->period));
  src = unpickle_io(src, (u32*)&(screen->val));
  src = unpickle_io(src, (u32*)&(screen->fill));
  src = unpickle_io(src, (u32*)&(screen->x));
  src = unpickle_io(src, (u32*)&(screen->y));

  if(screen->enable==1) {
      op_gfx_enable();
      op_screen_set_timer(screen);
      op_screen_redraw(screen);
  }

  return (u8*)src;
}

// redraw with current state
void op_screen_redraw(op_screen_t* screen) {
  if(screen->enable <= 0) { return; }

  // draw a pixel to the region, ha
  *(screen->reg.data + (screen->y * OP_SCREEN_PX_W) + screen->x) = screen->val;
  screen->reg.dirty = 1; 
}


// timer manipulation
static inline void op_screen_set_timer(op_screen_t* screen) {
  timers_set_custom(&(screen->timer), op_to_int(screen->period), &(screen->op_poll) );
}

static inline void op_screen_unset_timer(op_screen_t* screen) {
  timer_remove(&(screen->timer));
}
