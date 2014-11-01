// asf
#include "print_funcs.h"
// bees
#include "net_protected.h"
#include "op_cascades.h"

// LED intensity levels
#define L2 15
#define L1 9
#define L0 5



//-------------------------------------------------
//----- static variables

/////////////////
/// FIXME:
static io_t dummy = 0;
///////////////


//---- descriptor strings
static const char* op_cascades_instring = "FOCUS\0  SIZE\0   STEP\0   ";
static const char* op_cascades_outstring ="OUT0\0   OUT1\0   OUT2\0   OUT3\0   OUT4\0   OUT5\0   OUT6\0   OUT7\0   ";
static const char* op_cascades_opstring = "MP";

//-------------------------------------------------
//----- static function declaration

//---- input functions

//// network inputs: 
static void op_cascades_in_focus(op_cascades_t* grid, const io_t val);
static void op_cascades_in_size(op_cascades_t* grid, const io_t val);
static void op_cascades_in_step(op_cascades_t* grid, const io_t val);

// pickles
static u8* op_cascades_pickle(op_cascades_t* enc, u8* dst);
static const u8* op_cascades_unpickle(op_cascades_t* enc, const u8* src);

/// monome event handler
static void op_cascades_handler(op_monome_t* op_monome, u32 data);

static void op_cascades_redraw(void);
static void op_cascades_trigger(u8 n);

static u32 rnd(void);

// input func pointer array
static op_in_fn op_cascades_in_fn[3] = {
  (op_in_fn)&op_cascades_in_focus,
  (op_in_fn)&op_cascades_in_size,
  (op_in_fn)&op_cascades_in_step,
};


static s8 positions[8] = {3,1,2,2,3,3,5,7};
static s8 points[8] = {3,1,2,2,3,3,5,7};
static s8 points_save[8] = {3,1,2,2,3,3,5,7};
static u8 triggers[8] = {0,0,0,0,0,0,0,0};
static u8 trig_dests[8] = {0,0,0,0,0,0,0,0};
static u8 rules[8] = {0,0,0,0,0,0,0,0};
static u8 rule_dests[8] = {0,1,2,3,4,5,6,7};

static u8 edit_row, key_count = 0, mode = 0, prev_mode = 0;

static u8 XSIZE = 16;

const u8 glyph[8][8] = {{0,0,0,0,0,0,0,0},         // o
       {0,24,24,126,126,24,24,0},     // +
       {0,0,0,126,126,0,0,0},       // -
       {0,96,96,126,126,96,96,0},     // >
       {0,6,6,126,126,6,6,0},       // <
       {0,102,102,24,24,102,102,0},   // * rnd
       {0,120,120,102,102,30,30,0},   // <> up/down
       {0,126,126,102,102,126,126,0}};  // [] return


u32 a1 = 0x19660d;
u32 c1 = 0x3c6ef35f;
u32 x1 = 1234567;  // seed
u32 a2 = 0x19660d;
u32 c2 = 0x3c6ef35f;
u32 x2 = 7654321;  // seed



//-------------------------------------------------
//----- extern function definition
void op_cascades_init(void* mem) {
  //  print_dbg("\r\n op_cascades_init ");
  op_cascades_t* op = (op_cascades_t*)mem;

  // superclass functions
  //--- op
  op->super.in_fn = op_cascades_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_cascades_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_cascades_unpickle);

  //--- monome
  op->monome.handler = (monome_handler_t)&op_cascades_handler;
  op->monome.op = op;

  // superclass state

  op->super.type = eOpCascades;
  op->super.flags |= (1 << eOpFlagMonomeGrid);

  op->super.numInputs = 3;
  op->super.numOutputs = 8;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_cascades_opstring;
  op->super.inString = op_cascades_instring;
  op->super.outString = op_cascades_outstring;

  op->in_val[0] = &(op->focus);
  op->in_val[1] = &(op->size);  
  ///////////////////
  // FIXME
  op->in_val[2] = &dummy;  
  ////////////////////

  op->outs[0] = -1;
  op->outs[1] = -1;
  op->outs[2] = -1;
  op->outs[3] = -1;
  op->outs[4] = -1;
  op->outs[5] = -1;
  op->outs[6] = -1;
  op->outs[7] = -1;

  edit_row = key_count = mode = prev_mode = 0;

#ifdef BEEKEEP
#else
  op->size = monome_size_x();
#endif

  op->focus = OP_ONE;
  net_monome_set_focus(&(op->monome), 1);

  // init monome drawing
  op_cascades_redraw();
}

// de-init
void op_cascades_deinit(void* op) {
  // release focus
  net_monome_set_focus(&(((op_cascades_t*)op)->monome), 0);
}

//-------------------------------------------------
//----- static function definition

//--- network input functions
static void op_cascades_in_focus(op_cascades_t* op, const io_t v) {
  if((v) > 0) {
    op->focus = OP_ONE;
  } else {
    // if(op->focus>0) { net_monome_grid_clear(); }
    op->focus = 0;
  }
  net_monome_set_focus( &(op->monome), op->focus > 0);
}

static void op_cascades_in_size(op_cascades_t* op, const io_t v) {
  if(v < 9) op->size = 8;
  else op->size = 16;

  // FIXME: auto-detect size, this doesn't get set in unpickling
  XSIZE = op->size;
}

static void op_cascades_in_step(op_cascades_t* op, const io_t v) {
  u8 i;

  // clear last round
  for(i=0;i<8;i++)
    triggers[i] = 0;

  // main
  op_cascades_trigger(0);

  // ensure bounds, output triggers
  for(i=0;i<8;i++) {
    if(positions[i] < 0)
      positions[i] = 0;
    else if(positions[i] > points[i])
      positions[i] = points[i];

    // send out
    if(triggers[i]) {
      net_activate(op->outs[i], 1, op);
    }
  }

  op_cascades_redraw();
}

static void op_cascades_trigger(u8 n) {
  u8 m;

  positions[n]--;

  // ****** the trigger # check is so we don't cause a trigger/rules multiple times per NEXT
  // a rules-based jump to position-point does not current cause a trigger. should it?
  if(positions[n] < 0 && triggers[n] == 0) {
    triggers[n]++;
  
    if(rules[n] == 1) {     // inc
      if(points[rule_dests[n]] < (XSIZE-1)) {
        points[rule_dests[n]]++;
        positions[rule_dests[n]] = points[rule_dests[n]];
      }
    }
    else if(rules[n] == 2) {  // dec
      if(points[rule_dests[n]] > 0) {
        points[rule_dests[n]]--;
        positions[rule_dests[n]] = points[rule_dests[n]];
      }
    }
    else if(rules[n] == 3) {  // max
      points[rule_dests[n]] = (XSIZE-1);
      positions[rule_dests[n]] = points[rule_dests[n]];
    }
    else if(rules[n] == 4) {  // min
      points[rule_dests[n]] = 0;
      positions[rule_dests[n]] = points[rule_dests[n]];
    }
    else if(rules[n] == 5) {  // rnd
      points[rule_dests[n]] = rnd() % XSIZE;
      
      print_dbg("\r\n op_cascades >>>>>>>>>>>>>>>>>>>> RANDOM: ");
      print_dbg_hex(points[rule_dests[n]]);
      // print_dbg_hex(rnd() % 11);

      positions[rule_dests[n]] = points[rule_dests[n]];
    }
    else if(rules[n] == 6) {  // up/down
      points[rule_dests[n]] += rnd() % 3;
      points[rule_dests[n]]--;


      if(points[rule_dests[n]] < 0) points[rule_dests[n]] = 0;
      else if(points[rule_dests[n]] > (XSIZE-1)) points[rule_dests[n]] = XSIZE-1;
      positions[rule_dests[n]] = points[rule_dests[n]];  

      print_dbg("\r\n op_cascades >>>>>>>>>>>>>>>>>>>> WANDER: ");
      print_dbg_hex(points[rule_dests[n]]);   
    }
    else if(rules[n] == 7) {  // return
      points[rule_dests[n]] = points_save[rule_dests[n]];
    }


    //reset
    positions[n] += points[n] + 1;

    //triggers
    for(m=0;m<8;m++)
      if((trig_dests[n] & (1<<m)) != 0)
        op_cascades_trigger(m);
        // post("\ntrigger",n," -> ", m);
  }
}

// process monome key input
static void op_cascades_handler(op_monome_t* op_monome, u32 edata) {
  static u8 x, y, z;
  u8 dirty_grid = 0;

  // op_cascades_t* op = (op_cascades_t*)(op_monome->op);

  monome_grid_key_parse_event_data(edata, &x, &y, &z);

  prev_mode = mode;

  // mode check
  if(x == 0) {
    key_count += (z<<1)-1;

    if(key_count == 1 && z == 1)
      mode = 1; 
    else if(key_count == 0)
      mode = 0;

    if(z == 1 && mode == 1) {
      edit_row = y;
      dirty_grid = 1;
      // post("edit row:",edit_row);
    }
  }
  else if(x == 1 && mode != 0) {
    if(mode == 1 && z == 1)
      mode = 2;
    else if(mode == 2 && z == 0)
      mode = 1;
  }
  else if(mode == 0 && z == 1) {
    points[y] = x;
    points_save[y] = x;
    positions[y] = x;
    dirty_grid = 1;
  }
  else if(mode == 1 && z == 1) {
    if(y != edit_row) {    // filter out self-triggering
      trig_dests[edit_row] ^= (1<<y);
      dirty_grid = 1;
      // post("\ntrig_dests", edit_row, ":", trig_dests[edit_row]);
    }
  }
  else if(mode == 2 && z == 1) {
    if(x > 1 && x < 6) {
      rule_dests[edit_row] = y;
      dirty_grid = 1;
      // post("\nrule_dests", edit_row, ":", rule_dests[edit_row]);
    }
    else if(x > 5) {
      rules[edit_row] = y;
      dirty_grid = 1;
      // post("\nrules", edit_row, ":", rules[edit_row]);
    }
  }

  if(mode != prev_mode) {
    dirty_grid = 1;
    // post("\nnew mode", mode);
  }

  if(dirty_grid)
    op_cascades_redraw();

}

// redraw monome grid
static void op_cascades_redraw() {
  u8 i1, i2, i3;

  // clear grid
  for(i1=0;i1<128;i1++)
    monomeLedBuffer[i1] = 0;

  // SET POSITIONS
  if(mode == 0) {
    for(i1=0;i1<8;i1++) {
      for(i2=positions[i1];i2<=points[i1];i2++)
        monomeLedBuffer[i1*16 + i2] = L1;

      monomeLedBuffer[i1*16 + positions[i1]] = L2;
    }
  }
  // SET ROUTING
  else if(mode == 1) {
    monomeLedBuffer[edit_row * 16] = L1;
    monomeLedBuffer[edit_row * 16 + 1] = L1;

    for(i1=0;i1<8;i1++) {
      if((trig_dests[edit_row] & (1<<i1)) != 0) {
        for(i2=0;i2<=points[i1];i2++)
          monomeLedBuffer[i1*16 + i2] = L2;
      }
      monomeLedBuffer[i1*16 + positions[i1]] = L0;
    }
  }
  // SET RULES
  else if(mode == 2) {
    monomeLedBuffer[edit_row * 16] = L1;
    monomeLedBuffer[edit_row * 16 + 1] = L1;

    for(i1=2;i1<7;i1++)
      monomeLedBuffer[rule_dests[edit_row] * 16 + i1] = L2;

    for(i1=8;i1<16;i1++)
      monomeLedBuffer[rules[edit_row] * 16 + i1] = L0;

    for(i1=0;i1<8;i1++) 
      monomeLedBuffer[i1*16 + positions[i1]] = L0;

    for(i1=0;i1<8;i1++) {
      i3 = glyph[rules[edit_row]][i1];
      for(i2=0;i2<8;i2++) {
        if((i3 & (1<<i2)) != 0)
          monomeLedBuffer[i1*16 + 8 + i2] = L2;
      }
    }

    monomeLedBuffer[rules[edit_row] * 16 + 7] = L2;
  }

  monome_set_quadrant_flag(0);
  monome_set_quadrant_flag(1);
}


static u32 rnd() {
  x1 = x1 * c1 + a1;
  x2 = x2 * c2 + a2;
  return (x1>>16) | (x2>>16);
}




// pickle / unpickle
u8* op_cascades_pickle(op_cascades_t* mgrid, u8* dst) {
  dst = pickle_io(mgrid->focus, dst);
  dst = pickle_io(mgrid->size, dst);
  /// no state...???
  return dst;
}

const u8* op_cascades_unpickle(op_cascades_t* mgrid, const u8* src) {
  src = unpickle_io(src, (u32*)&(mgrid->focus));
  // FIXME should probably auto-detect grid size here::::
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
