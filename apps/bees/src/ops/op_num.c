// avr32-lib
#include "region.h"
#include "screen.h"

// bees
#include "net_protected.h"
#include "op_gfx.h"
#include "print_funcs.h"
#include "pickle.h"
#include "op_num.h"

//////  FIXME: trying with a full quad to start with
// width and height
#define OP_NUM_PX_W 64
#define OP_NUM_PX_H 32
#define OP_NUM_GFX_BYTES ( OP_NUM_PX_W * OP_NUM_PX_H )
// max offsets
#define OP_NUM_X_MAX ( SCREEN_ROW_PX - OP_NUM_PX_W ) 
#define OP_NUM_Y_MAX ( SCREEN_ROW_PX - OP_NUM_PX_H ) 

//-------------------------------------------------
//----- static vars

//-- descriptor
static const char* op_num_instring = "FOCUS   VAL     X       Y       ";
static const char* op_num_outstring = "";
static const char* op_num_opstring = "NUM";

//-- temp
// string buffer for number->text rendering
static char tmpStr[16];
// try sharing region data among instances...
static volatile u8 regData[OP_NUM_GFX_BYTES];
//// ^^^ fuck. i think that is broken. try moving to class and allocating from op poll?


//-------------------------------------------------
//----- static function declaration

// UI increment
static void op_num_inc(op_num_t* num, const s16 idx, const io_t inc);
// set inputs
static void op_num_in_focus(op_num_t* num, const io_t v);
static void op_num_in_val(op_num_t* num, const io_t v);
static void op_num_in_x(op_num_t* num, const io_t x);
static void op_num_in_y(op_num_t* num, const io_t y);

// pickle / unpickle
static u8* op_num_pickle(op_num_t* num, u8* dst);
static u8* op_num_unpickle(op_num_t* num, const u8* src);


//// redraw
static void op_num_redraw(op_num_t* num);

// array of input functions 
static op_in_fn op_num_in[4] = {
  (op_in_fn)&op_num_in_focus,
  (op_in_fn)&op_num_in_val,
  (op_in_fn)&op_num_in_x,
  (op_in_fn)&op_num_in_y,
};

//----- external function definition

/// initialize
void op_num_init(void* op) {
  op_num_t* num = (op_num_t*)op;

  // superclass functions
  num->super.inc_fn = (op_inc_fn)&op_num_inc;
  num->super.in_fn = op_num_in;
  num->super.pickle = (op_pickle_fn) (&op_num_pickle);
  num->super.unpickle = (op_unpickle_fn) (&op_num_unpickle);
  
  // superclass val
  num->super.numInputs = 4;
  num->super.numOutputs = 0;
 
  num->super.in_val = num->in_val;
  num->in_val[0] = &(num->focus);
  num->in_val[1] = &(num->val);
  num->in_val[2] = &(num->x);
  num->in_val[3] = &(num->y);

  num->super.out = num->outs;
  num->super.opString = op_num_opstring;
  num->super.inString = op_num_instring;
  num->super.outString = op_num_outstring;
  num->super.type = eOpNum;

  // class val
  num->focus = 0;
  num->val = 0;
  num->x = 0;
  num->y = 0;

  // init graphics
  /*.. this is sort of retarded, 
  doing stuff normally accomplished in region_alloc() or in static init,
  but doing a dumb memory hack on it to share a tmp data space between instances.
  */  
  num->reg.dirty = 0;
  num->reg.x = 0;
  num->reg.y = 0;
  num->reg.w = OP_NUM_PX_W;
  num->reg.h = OP_NUM_PX_H;
  num->reg.len = OP_NUM_GFX_BYTES;
  num->reg.data = (u8*) regData;

  region_fill(&(num->reg), 0);
}


/// de-initialize
void op_num_deinit(void* op) {
  op_num_t* num = (op_num_t*)op;
  if(num->focus > 0) {
    op_gfx_unfocus();
  }
}

//-------------------------------------------------
//----- static function definition

//===== operator input
void op_num_in_focus(op_num_t* num, const io_t v  ) {
  if(v > 0) {
    if(num->focus > 0) {
      ;;
    } else {
      op_gfx_focus();
      num->focus = 1;
    }
  } else { 
    if(num->focus > 0) {
      op_gfx_unfocus();
      num->focus = 0;
    } else {
      ;;
    }
  }
}

// input val
void op_num_in_val(op_num_t* num, const io_t v) {
  num->val = v;
  op_num_redraw(num);
}

// input x position
void op_num_in_x(op_num_t* num, const io_t v) {
  // blank so we don't leave a trail
  region_fill(&(num->reg), 0);
  if (v > OP_NUM_X_MAX) {
    num->x = num->reg.x = OP_NUM_X_MAX; 
  } else {		
    num->x = num->reg.x = v;
  }
  op_num_redraw(num);
}

// input y position
void op_num_in_y(op_num_t* num, const io_t v) {
  // blank so we don't leave a trail
  if (v > OP_NUM_Y_MAX) {
    num->y = num->reg.y = OP_NUM_Y_MAX;
  } else {		
    num->y = num->reg.y = v;
  }
  op_num_redraw(num);
}

//===== UI input

// increment
static void op_num_inc(op_num_t* num, const s16 idx, const io_t inc) {
  io_t val;
  switch(idx) {
  case 0: // focus
    op_num_in_focus(num, inc);
    break;
  case 1: // current value
    val = op_sadd(num->val, inc);
    op_num_in_val(num, val);
    break;
  case 2: // x position
    val = op_sadd(num->x, inc);
    op_num_in_x(num, val);
    break;
  case 3: // y position
    val = op_sadd(num->x, inc);
    op_num_in_y(num, val);
    break;
  }
}


// serialization
u8* op_num_pickle(op_num_t* num, u8* dst) {
  // store val variables
  dst = pickle_io(num->val, dst);
  dst = pickle_io(num->x, dst);
  dst = pickle_io(num->y, dst);
  return dst;
}

u8* op_num_unpickle(op_num_t* num, const u8* src) {
  // retreive val  variables
  src = unpickle_io(src, (u32*)&(num->val));
  src = unpickle_io(src, (u32*)&(num->x));
  src = unpickle_io(src, (u32*)&(num->y));
   return (u8*)src;
}

// redraw with current state
void op_num_redraw(op_num_t* num) {
  //// TEST
  /* u32 i, j; */
  /* u8* dat = &(num->reg.data[0]); */
  region* r = &(num->reg);

  if(num->focus <= 0) { return; }

  // print value to text buffer
  op_print(tmpStr, num->val);

  print_dbg("\r\n op_num_redraw , ");
  print_dbg(tmpStr);

  // blank
  region_fill(&(num->reg), 0);

  // render text to region
  region_string_aa(&(num->reg), tmpStr, 0, 0, 1);

  // ascii art
    /* print_dbg("\r\n"); */
    /* for(i=0; i< OP_NUM_PX_H; i++) { */
    /*   for(j=0; j< OP_NUM_PX_W; j++) { */
    /* 	if(*dat > 0) { print_dbg("#"); } else { print_dbg("_"); } */
    /* 	dat++; */
    /*   } */
    /*   print_dbg("\r\n"); */
    /* } */

    // FIXME: this should NOT go here. gfx ops should inherit from op_poll and only called for a redraw every 100 ms or whatever.
    screen_draw_region(r->x, r->y, r->w, r->h, r->data);
}
