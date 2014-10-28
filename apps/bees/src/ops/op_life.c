#include "print_funcs.h"

#include "net_protected.h"
#include "op_life.h"

//-------------------------------------------------
//----- static function declaration
static void op_life_in_next(op_life_t* life, const io_t v);
static void op_life_in_xsize(op_life_t* life, const io_t v);
static void op_life_in_ysize(op_life_t* life, const io_t v);
static void op_life_in_x(op_life_t* life, const io_t v);
static void op_life_in_y(op_life_t* life, const io_t v);
static void op_life_in_set(op_life_t* life, const io_t v);
static void op_life_in_noise(op_life_t* life, const io_t v);
static void op_life_in_rules(op_life_t* life, const io_t v);
static void op_life_in_focus(op_life_t* life, const io_t v);

static void op_life_output(op_life_t* life);

// life
static void life_change(u8,u8);
static void life_init(void);
static u8 neighbors(u8,u8,u16);

// monome event handler
static void op_life_handler(op_monome_t* op_monome, u32 data);

// pickle / unpickle
static u8* op_life_pickle(op_life_t* op, u8* dst);
static const u8* op_life_unpickle(op_life_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_life_in_fn[9] = {
  (op_in_fn)&op_life_in_next,
  (op_in_fn)&op_life_in_xsize,
  (op_in_fn)&op_life_in_ysize,
  (op_in_fn)&op_life_in_x,
  (op_in_fn)&op_life_in_y,
  (op_in_fn)&op_life_in_set,
  (op_in_fn)&op_life_in_noise,
  (op_in_fn)&op_life_in_rules,
  (op_in_fn)&op_life_in_focus
};

static const char* op_life_instring  = "NEXT\0   XSIZE\0  YSIZE\0  X\0      Y\0      SET\0    NOISE\0  RULES\0  FOCUS\0  ";
static const char* op_life_outstring = "VAL\0    POP\0    DELTA\0  ";
static const char* op_life_opstring  = "LIFE";


s8 lifenow[256];
s8 lifenext[256];

//-------------------------------------------------
//----- external function definitions
void op_life_init(void* mem) {
  op_life_t* life = (op_life_t*)mem;

  //--- monome
  life->monome.handler = (monome_handler_t)&op_life_handler;
  life->monome.op = life;

  life->super.numInputs = 9;
  life->super.numOutputs = 3;
  life->outs[0] = -1;
  life->outs[1] = -1;
  life->outs[2] = -1;

  life->super.in_fn = op_life_in_fn;
  life->super.pickle = (op_pickle_fn) (&op_life_pickle);
  life->super.unpickle = (op_unpickle_fn) (&op_life_unpickle);

  life->super.in_val = life->in_val;
  life->super.out = life->outs;
  life->super.opString = op_life_opstring;
  life->super.inString = op_life_instring;
  life->super.outString = op_life_outstring;
  life->super.type = eOpLife;
  life->super.flags |= (1 << eOpFlagMonomeGrid);
  
  life->in_val[0] = &(life->next);
  life->in_val[1] = &(life->xsize);
  life->in_val[2] = &(life->ysize);
  life->in_val[3] = &(life->x);
  life->in_val[4] = &(life->y);
  life->in_val[5] = &(life->set);
  life->in_val[6] = &(life->noise);
  life->in_val[7] = &(life->rules);
  life->in_val[8] = &(life->focus);

  life->next = 0;
  //??? FIXME
#ifdef BEEKEEP
#else
  life->xsize = monome_size_x();
  life->ysize = monome_size_y();
#endif

  life->x = 0;
  life->y = 0;
  life->set = 0;
  life->noise = 0;
  life->rules = 0;
  life->focus = 1;

  life->pop = life->lpop = 0;

  life_init();

  net_monome_set_focus(&(life->monome), 1);
}

// de-init
void op_life_deinit(void* op) {
  // release focus
  net_monome_set_focus(&(((op_life_t*)op)->monome), 0);
}

//-------------------------------------------------
//----- static function definitions
static void op_life_in_focus(op_life_t* op, const io_t v) {
  if((v) > 0) {
    op->focus = OP_ONE;
  } else {
    op->focus = 0;;
  }
  net_monome_set_focus( &(op->monome), op->focus > 0);
}

static void op_life_in_next(op_life_t* life, const io_t v) {
  if(v!=0) {
    u8 i, x, y, count;
    u8 *p = monomeLedBuffer;

    for(x=0;x<life->xsize;x++)
    {
      for(y=0;y<life->ysize;y++)
      {
        i = x+(y<<4);
        if(lifenext[i]==1)
        {
          lifenow[i]=1;
          p[i]=15;
        }
        else if(lifenext[i]==-1)
        {
          lifenow[i]=0;
          p[i]=0;
        }
        lifenext[i]=0;
      }
    }

    for(x=0; x < life->xsize; x++)
    { 
      for(y=0; y < life->ysize; y++) 
      { 
        i = x+(y<<4);
        count = neighbors(x, y, life->xsize * life->ysize);

        if (count == 3 && lifenow[i] == 0 && life->rules != 2) 
          lifenext[i]=1;
        if ((count < 2 || count > 3) && lifenow[i] == 1 && life->rules != 1) 
        lifenext[i]=-1;  
      } 
    } 

    // FIXME: OPTIMIZE
    monome_set_quadrant_flag(0);

    if(life->xsize>8) monome_set_quadrant_flag(1);
    if(life->ysize>8) { monome_set_quadrant_flag(2); monome_set_quadrant_flag(3); }

    op_life_output(life);
  }
}

static void op_life_in_xsize(op_life_t* life, const io_t v) {
  if(v < 9) life->xsize = 8;
  else life->xsize = 16;
}

static void op_life_in_ysize(op_life_t* life, const io_t v) {
  if(v < 9) life->ysize = 8;
  else life->ysize = 16;
}

static void op_life_in_x(op_life_t* life, const io_t v) {
  if(v < 0) life->x = 0;
  else if(v > life->xsize) life->x = life->xsize;
  else life->x = v;

  net_activate(life->outs[0], lifenow[life->x + (life->y << 4)], life);
}

static void op_life_in_y(op_life_t* life, const io_t v) {
  if(v < 0) life->y = 0;
  else if(v > life->ysize) life->y = life->ysize;
  else life->y = v;

  net_activate(life->outs[0], lifenow[life->x + (life->y << 4)], life);
}

static void op_life_in_set(op_life_t* life, const io_t v) {
  u8 *p = monomeLedBuffer;
  u8 i = life->x+(life->y<<4);

  if(v == 0) lifenow[i] = 0;
  else lifenow[i] = 1;

  p[i]=lifenow[i] * 15;
  monome_calc_quadrant_flag(life->x, life->y);

  op_life_output(life);
}

static void op_life_in_noise(op_life_t* life, const io_t v) {
  return;
}

static void op_life_in_rules(op_life_t* life, const io_t v) {
  if(v < 1) life->rules = 0;
  else if(v > 1) life->rules = 2;
  else life->rules = v;
}

// ===== monome handler
static void op_life_handler(op_monome_t* op_monome, u32 edata) {
  static u8 x, y, z;

  monome_grid_key_parse_event_data(edata, &x, &y, &z);

  if(z) life_change(x,y);
}


static void op_life_output(op_life_t* life) {
  net_activate(life->outs[0], lifenow[life->x + (life->y << 4)], life);

  life->lpop = life->pop;
  life->pop = 0;
  for(u16 i=0;i<256;i++) life->pop += lifenow[i];
  net_activate(life->outs[1], life->pop, life);
  net_activate(life->outs[2], (life->pop - life->lpop), life);
}


static void life_init(void) {
  u16 i;
  for(i=0;i<256;i++)
  {
    lifenow[i] = 0;
    lifenext[i] = 0;  
  }
}

static void life_change(u8 x,u8 y) {
  u8 *p = monomeLedBuffer;
  u8 i = x+(y<<4);
  lifenow[i] ^= 1;
  p[i]=lifenow[i] * 15;
  monome_calc_quadrant_flag(x, y);
}

static u8 neighbors(u8 x, u8 y, u16 s)
{
  if(s==64)
    return lifenow[((x + 1) % 8) + ((y)<<4)] + 
      lifenow[(x) + (((y + 1) % 8)<<4)] + 
      lifenow[((x + 8 - 1) % 8) + ((y)<<4)] + 
      lifenow[(x) + (((y + 8 - 1) % 8)<<4)] + 
      lifenow[((x + 1) % 8) + (((y + 1) % 8)<<4)] + 
      lifenow[((x + 8 - 1) % 8) + (((y + 1) % 8)<<4)] + 
      lifenow[((x + 8 - 1) % 8) + (((y + 8 - 1) % 8)<<4)] + 
      lifenow[((x + 1) % 8) + (((y + 8 - 1) % 8)<<4)];
  else if(s==256)
    return lifenow[((x + 1) % 16) + ((y)<<4)] + 
      lifenow[(x) + (((y + 1) % 16)<<4)] + 
      lifenow[((x + 16 - 1) % 16) + ((y)<<4)] + 
      lifenow[(x) + (((y + 16 - 1) % 16)<<4)] + 
      lifenow[((x + 1) % 16) + (((y + 1) % 16)<<4)] + 
      lifenow[((x + 16 - 1) % 16) + (((y + 1) % 16)<<4)] + 
      lifenow[((x + 16 - 1) % 16) + (((y + 16 - 1) % 16)<<4)] + 
      lifenow[((x + 1) % 16) + (((y + 16 - 1) % 16)<<4)];
  else
    return lifenow[((x + 1) % 16) + ((y)<<4)] + 
      lifenow[(x) + (((y + 1) % 8)<<4)] + 
      lifenow[((x + 16 - 1) % 16) + ((y)<<4)] + 
      lifenow[(x) + (((y + 8 - 1) % 8)<<4)] + 
      lifenow[((x + 1) % 16) + (((y + 1) % 8)<<4)] + 
      lifenow[((x + 16 - 1) % 16) + (((y + 1) % 8)<<4)] + 
      lifenow[((x + 16 - 1) % 16) + (((y + 8 - 1) % 8)<<4)] + 
      lifenow[((x + 1) % 16) + (((y + 8 - 1) % 8)<<4)];
}

// pickle / unpickle
u8* op_life_pickle(op_life_t* op, u8* dst) {
  dst = pickle_io(op->xsize, dst);
  dst = pickle_io(op->ysize, dst);
  dst = pickle_io(op->x, dst);
  dst = pickle_io(op->y, dst);
  dst = pickle_io(op->rules, dst);
  dst = pickle_io(op->focus, dst);
  return dst;
}

const u8* op_life_unpickle(op_life_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->xsize));
  src = unpickle_io(src, &(op->ysize));
  src = unpickle_io(src, &(op->x));
  src = unpickle_io(src, &(op->y));
  src = unpickle_io(src, &(op->rules));
  src = unpickle_io(src, &(op->focus));
  return src;
}
