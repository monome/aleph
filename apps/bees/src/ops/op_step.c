// asf
#include "print_funcs.h"
// bees
#include "net_protected.h"
#include "op_step.h"

//-------------------------------------------------
//----- static variables

//---- descriptor strings
static const char* op_step_instring = "FOCUS\0  SIZE\0   STEP\0   ";
static const char* op_step_outstring ="A\0      B\0      C\0      D\0      MONO1\0  POS1\0   MONO2\0  POS2\0   ";
static const char* op_step_opstring = "STEP";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_step_in_focus(op_step_t* grid, const io_t val);
static void op_step_in_size(op_step_t* grid, const io_t val);
static void op_step_in_step(op_step_t* grid, const io_t val);

// pickles
static u8* op_step_pickle(op_step_t* enc, u8* dst);
static const u8* op_step_unpickle(op_step_t* enc, const u8* src);

/// monome event handler
static void op_step_handler(op_monome_t* op_monome, u32 data);

// input func pointer array
static op_in_fn op_step_in_fn[3] = {
  (op_in_fn)&op_step_in_focus,
  (op_in_fn)&op_step_in_size,
  (op_in_fn)&op_step_in_step,
};

//-------------------------------------------------
//----- extern function definition
void op_step_init(void* mem) {
  u8 i;
  //  print_dbg("\r\n op_step_init ");
  op_step_t* op = (op_step_t*)mem;

  // superclass functions
  //--- op
  op->super.in_fn = op_step_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_step_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_step_unpickle);

  //--- monome
  op->monome.handler = (monome_handler_t)&op_step_handler;
  op->monome.op = op;

  // superclass state

  op->super.type = eOpStep;
  op->super.flags |= (1 << eOpFlagMonomeGrid);

  op->super.numInputs = 3;
  op->super.numOutputs = 8;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_step_opstring;
  op->super.inString = op_step_instring;
  op->super.outString = op_step_outstring;

  op->in_val[0] = &(op->focus);
  op->in_val[1] = &(op->size);  
  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;
  op->outs[3] = -1;
  op->outs[4] = -1;
  op->outs[5] = -1;
  op->outs[6] = -1;
  op->outs[7] = -1;

  op->s_start = 0;
  op->s_end = 7;
  op->s_length = 7;
  op->s_now = 0;
  op->s_cut = 0;

  op->s_start2 = 0;
  op->s_end2 = 7;
  op->s_length2 = 7;
  op->s_now2 = 0;
  op->s_cut2 = 0;

  for(i=0;i<16;i++) {
    op->steps[0][i] = 0;
    op->steps[1][i] = 0;
    op->steps[2][i] = 0;
    op->steps[3][i] = 0;
  }

#ifdef BEEKEEP
#else
  op->size = monome_size_x();
#endif

  op->focus = 0; //OP_ONE;
  //net_monome_set_focus(&(op->monome), 1);

  // init monome drawing, maybe should clear first
  monomeLedBuffer[monome_xy_idx(0, 0)] = 15;
  monomeLedBuffer[monome_xy_idx(0, 2)] = 15;
  for(i=0;i<op->size;i++) {
    monomeLedBuffer[monome_xy_idx(i, 1)] = 15;
    monomeLedBuffer[monome_xy_idx(i, 3)] = 15;
  }
  monome_set_quadrant_flag(0);

}

// de-init
void op_step_deinit(void* op) {
  // release focus
  net_monome_set_focus(&(((op_step_t*)op)->monome), 0);
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_step_in_focus(op_step_t* op, const io_t v) {
  if((v) > 0) {
    op->focus = OP_ONE;
  } else {
    // if(op->focus>0) { net_monome_grid_clear(); }
    op->focus = 0;
  }
  net_monome_set_focus( &(op->monome), op->focus > 0);
}

static void op_step_in_size(op_step_t* op, const io_t v) {
  if(v < 9) op->size = 8;
  else op->size = 16;
}

static void op_step_in_step(op_step_t* op, const io_t v) {
  s8 i;

  if(op->s_cut == 0) {
    monomeLedBuffer[monome_xy_idx(op->s_now, 0)] = 0;

    if(v > 0) {
      for(i=0;i<v;i++) {
        if(op->s_now == op->s_end) op->s_now = op->s_start;
        else {
          op->s_now++;
          if(op->s_now == op->size) op->s_now = 0;
        }
      }
    } else {
      for(i=v;i<0;i++) {
        if(op->s_now == op->s_start) op->s_now = op->s_end;
        else if(op->s_now == 0) op->s_now = op->size - 1;
        else op->s_now--;
      }
    }

    monomeLedBuffer[monome_xy_idx(op->s_now, 0)] = 15;
    monome_set_quadrant_flag(0);
    monome_set_quadrant_flag(1);
  }

  if(op->s_cut2 == 0) {
    monomeLedBuffer[monome_xy_idx(op->s_now2, 2)] = 0;

    if(v > 0) {
      for(i=0;i<v;i++) {
        if(op->s_now2 == op->s_end2) op->s_now2 = op->s_start2;
        else {
          op->s_now2++;
          if(op->s_now2 == op->size) op->s_now2 = 0;
        }
      }
    } else {
      for(i=v;i<0;i++) {
        if(op->s_now2 == op->s_start2) op->s_now2 = op->s_end2;
        else if(op->s_now2 == 0) op->s_now2 = op->size - 1;
        else op->s_now2--;
      }
    }

    monomeLedBuffer[monome_xy_idx(op->s_now2, 2)] = 15;
    monome_set_quadrant_flag(0);
    monome_set_quadrant_flag(1);
  }
  
  op->s_cut = 0;
  op->s_cut2 = 0;

  net_activate(op->outs[0], op->steps[0][op->s_now], op);
  net_activate(op->outs[1], op->steps[1][op->s_now], op);
  net_activate(op->outs[2], op->steps[2][op->s_now], op);
  net_activate(op->outs[3], op->steps[3][op->s_now], op);

  i = (op->steps[0][op->s_now]) + (op->steps[1][op->s_now] << 1) + (op->steps[2][op->s_now] << 2) + (op->steps[3][op->s_now] << 3);
  net_activate(op->outs[4], i, op);
  net_activate(op->outs[5], op->s_now, op);

  i = (op->steps[0][op->s_now2]) + (op->steps[1][op->s_now2] << 1) + (op->steps[2][op->s_now2] << 2) + (op->steps[3][op->s_now2] << 3);
  net_activate(op->outs[6], i, op);
  net_activate(op->outs[7], op->s_now2, op);
}

static void op_step_handler(op_monome_t* op_monome, u32 edata) {
  static u8 x, y, z;
  u8 i;

  op_step_t* op = (op_step_t*)(op_monome->op);


  monome_grid_key_parse_event_data(edata, &x, &y, &z);

  // only care about key-downs
  if(z) {
    // row 0 = postion cut, set start point
    if(y==0) {
      op->s_start = x;
      monomeLedBuffer[monome_xy_idx(op->s_now, 0)] = 0;
      op->s_now = x;
      monomeLedBuffer[monome_xy_idx(op->s_now, 0)] = 15;

      op->s_end = op->s_start + op->s_length;
      if(op->s_end > (op->size-1)) op->s_end -= op->size;

      if(op->s_end >= op->s_start)
        for(i=0;i<op->size;i++) {
          monomeLedBuffer[monome_xy_idx(i, 1)] = (i >= op->s_start && i <= op->s_end) * 15;
        }
      else {
        for(i=0;i<op->size;i++) {
          monomeLedBuffer[monome_xy_idx(i, 1)] = (i >= op->s_start || i <= op->s_end) * 15;
        }
      }

      monome_set_quadrant_flag(0);
      if(op->size>8) monome_set_quadrant_flag(1);


      op->s_cut = 1;

    // row 1 = change loop point
    } else if(y==1) {
      op->s_end = x;
      op->s_length = op->s_end - op->s_start;
      if(op->s_length < 0) op->s_length += op->size;

      if(op->s_end >= op->s_start)
        for(i=0;i<op->size;i++) {
          monomeLedBuffer[monome_xy_idx(i, 1)] = (i >= op->s_start && i <= op->s_end) * 15;
        }
      else {
        for(i=0;i<op->size;i++) {
          monomeLedBuffer[monome_xy_idx(i, 1)] = (i >= op->s_start || i <= op->s_end) * 15;
        }
      }

      monome_set_quadrant_flag(0);
      if(op->size>8) monome_set_quadrant_flag(1);

    // set loop start 2
    } else if(y==2) {
      op->s_start2 = x;
      monomeLedBuffer[monome_xy_idx(op->s_now2, 2)] = 0;
      op->s_now2 = x;
      monomeLedBuffer[monome_xy_idx(op->s_now2, 2)] = 15;

      op->s_end2 = op->s_start2 + op->s_length2;
      if(op->s_end2 > (op->size-1)) op->s_end2 -= op->size;

      if(op->s_end2 >= op->s_start2)
        for(i=0;i<op->size;i++) {
          monomeLedBuffer[monome_xy_idx(i, 3)] = (i >= op->s_start2 && i <= op->s_end2) * 15;
        }
      else {
        for(i=0;i<op->size;i++) {
          monomeLedBuffer[monome_xy_idx(i, 3)] = (i >= op->s_start2 || i <= op->s_end2) * 15;
        }
      }

      monome_set_quadrant_flag(0);
      if(op->size>8) monome_set_quadrant_flag(1);


      op->s_cut2 = 1;

    // row 3 = change loop point 2
    } else if(y==3) {
      op->s_end2 = x;
      op->s_length2 = op->s_end2 - op->s_start2;
      if(op->s_length2 < 0) op->s_length2 += op->size;

      if(op->s_end2 >= op->s_start2)
        for(i=0;i<op->size;i++) {
          monomeLedBuffer[monome_xy_idx(i, 3)] = (i >= op->s_start2 && i <= op->s_end2) * 15;
        }
      else {
        for(i=0;i<op->size;i++) {
          monomeLedBuffer[monome_xy_idx(i, 3)] = (i >= op->s_start2 || i <= op->s_end2) * 15;
        }
      }

      monome_set_quadrant_flag(0);
      if(op->size>8) monome_set_quadrant_flag(1);


    // rows 4-7: set steps
    } else if(y>3 && y<8) {
      op->steps[y-4][x] ^= 1;
      monomeLedBuffer[monome_xy_idx(x, y)] = op->steps[y-4][x] * 15;
      monome_calc_quadrant_flag(x, y);
    }
  }
}



// pickle / unpickle
u8* op_step_pickle(op_step_t* mgrid, u8* dst) {
  dst = pickle_io(mgrid->focus, dst);
  dst = pickle_io(mgrid->size, dst);
  /// no state...???
  return dst;
}

const u8* op_step_unpickle(op_step_t* mgrid, const u8* src) {
  src = unpickle_io(src, (u32*)&(mgrid->focus));
  src = unpickle_io(src, (u32*)&(mgrid->size));
  /*
    probably shouldn't call this here...
   if we assume that network monome device focus is null during unpickling,
   it will be ok.  that assumption should hold true, but if it doesn't, 
   or if we change something and forget to update this,
   the result is both and hard to track (dereferencing a garbage pointer.)
   we should just explicitly check for focused grid ops after scene recall, last one wins...
  */
  net_monome_set_focus( &(mgrid->monome), mgrid->focus > 0);
  return src;
}
