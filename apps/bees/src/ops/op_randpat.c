//op_randpat - produces a 8 note random midi pattern

//asf
#include "print_funcs.h"
//Bees
#include "app_timers.h"
#include "net_poll.h"
#include "net_protected.h"
#include "op_randpat.h"

//--------------------------------------------------
//----Static Variables
//Descriptor Variables
static const char* op_randpat_instring = "CHAN\0   TICK\0   VEL\0    MIN\0    MAX\0    RND\0    ENABLE\0 PERIOD\0   ";
static const char* op_randpat_outstring = "NOTE\0   ";
static const char* op_randpat_opstring = "RNDPT";

//--------------------------------------------------
//----Static Function Decleration

//--Input Functions
//Network Inputs
static void op_randpat_in_chan(op_randpat_t* op, const io_t val);
static void op_randpat_in_tick(op_randpat_t* op, const io_t val);
static void op_randpat_in_vel(op_randpat_t* op, const io_t val);
static void op_randpat_in_min(op_randpat_t* op, const io_t val);
static void op_randpat_in_max(op_randpat_t* op, const io_t val);
static void op_randpat_in_rnd(op_randpat_t* op, const io_t val);
static void op_randpat_in_enable(op_randpat_t* op, const io_t val);
static void op_randpat_in_period(op_randpat_t* op, const io_t val);

//Pickles
static u8* op_randpat_pickle(op_randpat_t* enc, u8* dst);
static const u8* op_randpat_unpickle(op_randpat_t* enc, const u8* src);

//Timer Manipulation
static inline void op_randpat_set_timer(op_randpat_t* op);
static inline void op_randpat_unset_timer(op_randpat_t* op);

// polled-operator handler
void op_randpat_poll_handler(void* op);

//Build and Send a Midi Serial Packet
static void op_randpat_send_packet( op_randpat_t* op );

//Input Function Pointer Array
static op_in_fn op_randpat_in_fn[8] = {
  (op_in_fn)&op_randpat_in_chan,
  (op_in_fn)&op_randpat_in_tick,
  (op_in_fn)&op_randpat_in_vel,
  (op_in_fn)&op_randpat_in_min,
  (op_in_fn)&op_randpat_in_max,
  (op_in_fn)&op_randpat_in_rnd,
  (op_in_fn)&op_randpat_in_enable,
  (op_in_fn)&op_randpat_in_period,
};

int randomnumcreator(void);//Random number creation

int array[8];

//--------------------------------------------------
//----External Function Definition
void op_randpat_init(void* mem) {
  op_randpat_t* op = (op_randpat_t*)mem;

  //Superclass Functions
  //--op
  op->super.in_fn = op_randpat_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_randpat_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_randpat_unpickle);

  //Superclass State
  op->super.type = eOpRandpat;

  op->super.numInputs = 8;
  op->super.numOutputs = 1;
  op->outs[0] = -1;

  //Polled operator Superclass
  op->op_poll.handler = (poll_handler_t)(&op_randpat_poll_handler);
  op->op_poll.op = op;

  op->super.in_val = op->in_val;
  op->super.out = op->outs;

  op->super.opString = op_randpat_opstring;
  op->super.inString = op_randpat_instring;
  op->super.outString = op_randpat_outstring;

  op->in_val[0] = &(op->chan);
  op->in_val[1] = &(op->tick); 
  op->in_val[2] = &(op->vel);
  op->in_val[3] = &(op->min);
  op->in_val[4] = &(op->max);
  op->in_val[5] = &(op->rnd);
  op->in_val[6] = &(op->enable);
  op->in_val[7] = &(op->period);

  //Start
  op->tick = 0;
  op->vel = 100;
  op->min = 1;
  op->max = 127;
  op->rnd = 0;
  op->enable = 0;
  op->period = op_from_int(125);

  //Timer (unlinked)
  op->timer.next = NULL;

}

//Values for Random Number Creation
u32 a = 0x19660d, c = 0x3c6ef35f, x = 111; 

int minval = 0, maxval = 127, notes, arraypos = 0, playpoint = 1;

//Deinit
void op_randpat_deinit(void* op) {
    op_randpat_unset_timer(op);
};

//--------------------------------------------------
//----Static Function Definitions
static void op_randpat_in_tick(op_randpat_t* op, const io_t v) {
  op->tick = v;
  if(op->tick == 1) {
    if(arraypos == 7){notes = array[arraypos]; arraypos = 0;}
    else{notes = array[arraypos]; arraypos++;}
    op_randpat_send_packet(op);
  };
}

static void op_randpat_in_vel(op_randpat_t* op, const io_t v) {
  if(v < 0) { op->vel = 0; }
  else if (v > 127) { op->vel = 127; }
  else {op->vel = v;}
}

static void op_randpat_in_min(op_randpat_t* op, const io_t v) {
  if(v < 0) {op->min = 0; }
  else if(v > (op->max - 1)) {op->min = (op->max - 1);}
  else {op->min = v;}
  minval = v;
}

static void op_randpat_in_max(op_randpat_t* op, const io_t v) {
  if(v < (op->min + 1)) {op->max = (op->min + 1);}
  else if(v > 127) {op->max = 127;}
  else {op->max = v;}
  maxval = v;
}

static void op_randpat_in_rnd(op_randpat_t* op, const io_t val) {
  if((val) > 0) { 
    int i;
    for(i = 0; i < 8; i++) {array[i] = randomnumcreator();}
    op->rnd = 0; 
  }
  else{op->rnd = 0;}
}

static void op_randpat_in_chan(op_randpat_t* op, const io_t v) {
  if(v < -1) { op->chan = -1; }
  else if(v > 15) { op->chan = 15; } 
  else { op->chan = v; }
}

//Input Enable/Disable
void op_randpat_in_enable(op_randpat_t* op, const io_t v) {
  if((v) > 0) {
    if(op->enable == 0) { op->enable = OP_ONE; op_randpat_set_timer(op);}
  } 
  else {
    if(op->enable > 0) { op->enable = 0; op_randpat_unset_timer(op);}
  }
}

void op_randpat_in_period(op_randpat_t* op, const io_t v) {
  if((v) < 5) { op->period = 5;} 
  else {op->period = v;}
  op->timer.ticks = op_to_int(op->period);
}

//Build and Send a Midi Serial Packet
void op_randpat_send_packet( op_randpat_t* op ) {
  u8 pack[3];
  if(op->vel == 0) {pack[0] = 0x80;} //Note On/Off
  else { pack[0] = 0x90;}

  pack[0] |= (u8)(op->chan & 0x0f);
  pack[1] = (u8)(notes);
  pack[2] = (u8)(op->vel);

  midi_write(pack, 3);

  net_activate(op->outs[0], notes, op);//Print the outputted midi note out
}

int randomnumcreator() {
  int val;
  x = x * c + a;
  val = x;
  if(val < 0)
    val *= -1;
  val = (val % ((maxval - minval) + 1)) + minval;
  return val;
}

//Poll Event Handler
void op_randpat_poll_handler(void* op) {
  if(playpoint == 1) {
    if(arraypos == 7){notes = array[arraypos]; arraypos = 0;}
    else{notes = array[arraypos]; arraypos++;}
  };
  op_randpat_send_packet(op);
}

//Pickle/Unpickle
u8* op_randpat_pickle(op_randpat_t* op, u8* dst) {
  dst = pickle_io(op->tick, dst);
  return dst;
}

const u8* op_randpat_unpickle(op_randpat_t* op, const u8* src) {
  src = unpickle_io(src, (u32*)&(op->tick));
  if(op->enable) { op_randpat_set_timer(op); }
  return src;
}

//--------------------------------------------------
//Timer Manipulation
static inline void op_randpat_set_timer(op_randpat_t* op) {
  timers_set_custom(&(op->timer), op_to_int(op->period), &(op->op_poll) );
}

static inline void op_randpat_unset_timer(op_randpat_t* op) {
  timer_remove(&(op->timer));
}
