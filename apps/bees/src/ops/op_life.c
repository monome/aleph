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
static void op_life_in_tog(op_life_t* life, const io_t v);
static void op_life_in_rules(op_life_t* life, const io_t v);

// life
static void life_init(void);
static u8 neighbors(u8,u8,u8,u8);

// pickle / unpickle
static u8* op_life_pickle(op_life_t* op, u8* dst);
static const u8* op_life_unpickle(op_life_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_life_in_fn[8] = {
  (op_in_fn)&op_life_in_x,
  (op_in_fn)&op_life_in_y,
  (op_in_fn)&op_life_in_set,
  (op_in_fn)&op_life_in_tog,
  (op_in_fn)&op_life_in_next,
  (op_in_fn)&op_life_in_xsize,
  (op_in_fn)&op_life_in_ysize,
  (op_in_fn)&op_life_in_rules,
};

static const char* op_life_instring  = "X\0      Y\0      SET\0    TOG\0    NEXT\0   XSIZE\0  YSIZE\0  RULES\0  ";
static const char* op_life_outstring = "X\0      Y\0      VAL\0    POP\0    ";
static const char* op_life_opstring  = "LIFE";


s8 lifenow[16][16];
s8 lifenext[16][16];

//-------------------------------------------------
//----- external function definitions
void op_life_init(void* mem) {
  op_life_t* life = (op_life_t*)mem;

  //--- monome
  life->super.numInputs = 8;
  life->super.numOutputs = 4;
  life->outs[0] = -1;
  life->outs[1] = -1;
  life->outs[2] = -1;
  life->outs[3] = -1;

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
  
  life->in_val[0] = &(life->x);
  life->in_val[1] = &(life->y);
  life->in_val[2] = &(life->set);
  life->in_val[3] = &(life->tog);
  life->in_val[4] = &(life->next);
  life->in_val[5] = &(life->xsize);
  life->in_val[6] = &(life->ysize);
  life->in_val[7] = &(life->rules);

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
  life->tog = 0;
  life->rules = 0;

  life->pop = life->lpop = 0;

  life_init();

}

// de-init
void op_life_deinit(void* op) {
}

//-------------------------------------------------
//----- static function definitions

static inline void output_cell (op_life_t* life, u8 x, u8 y, u8 z) {
  net_activate(life->outs[0], x, life);
  net_activate(life->outs[1], y, life);
  net_activate(life->outs[2], z, life);
}

static void op_life_in_next(op_life_t* life, const io_t v) {
  if(v != 0) {
    life->next = 1;
    u8 x, y, count;
    for(x=0; x < life->xsize; x++) {
      for(y=0; y < life->ysize; y++) {
        count = neighbors(x, y, life->xsize, life->ysize);

	//Any dead cell with exactly 3 neighors comes alive
        if (count == 3 &&
	    lifenow[x][y] == 0 &&
	    life->rules != 2)
          lifenext[x][y]=1;
	//Any live cell with < 2 neighbors or > 3 neighbors dies
        else if ((count < 2 || count > 3) &&
		 lifenow[x][y] == 1 &&
		 life->rules != 1)
	  lifenext[x][y] = 0 ;
	//Any other cell stays the same
	else
	  lifenext[x][y] = lifenow[x][y];
      }
    }
    u8 pop = 0;
    for(x=0; x < life->xsize; x++) {
      for(y=0; y < life->ysize; y++) {
    	//Draw cells that have changed
	if (lifenext[x][y] != lifenow[x][y])
	  output_cell(life, x, y, lifenext[x][y]);
	lifenow[x][y] = lifenext[x][y];

	//increment population counter
	pop += lifenext[x][y];
      }
    }

    net_activate(life->outs[3], pop, life);
  }
  else
    life->next = 0;
}

static void op_life_in_xsize(op_life_t* life, const io_t v) {
  if(v > 16)
    life->xsize = 16;
  if (v < 0)
    life->xsize = 0;
  else
    life->xsize = v;
}

static void op_life_in_ysize(op_life_t* life, const io_t v) {
  if(v > 16)
    life->ysize = 16;
  if (v < 0)
    life->ysize = 0;
  else
    life->ysize = v;
}

static void op_life_in_x(op_life_t* life, const io_t v) {
  if (v >= 0 && v < life->xsize)
    life->x = v;
}

static void op_life_in_y(op_life_t* life, const io_t v) {
  if (v >= 0 && v < life->ysize)
  life->y = v;
}

static void op_life_in_set(op_life_t* life, const io_t v) {
  if (v <= 0)
    lifenow[life->x][life->y] = 0;
  else
    lifenow[life->x][life->y] = 1;
  output_cell(life, life->x, life->y, lifenow[life->x][life->y]);
}

static void op_life_in_tog(op_life_t* life, const io_t v) {
  if (v != 0) {
    lifenow[life->x][life->y] = ! lifenow[life->x][life->y];
    output_cell(life, life->x, life->y, lifenow[life->x][life->y]);
  }
}

static void op_life_in_rules(op_life_t* life, const io_t v) {
  if (v < 1)
    life->rules = 0;
  else if (v > 1)
    life->rules = 2;
  else
    life->rules = v;
}

static void life_init(void) {
  int x, y;
  for(x = 0; x < 16; x++) {
    for(y = 0; y < 16; y++) {
      lifenow[x][y] = 0;
      lifenext[x][y] = 0;
    }
  }
}

static u8 neighbors(u8 x, u8 y, u8 xsize, u8 ysize) {
  u8 x_ = (x + 1) % xsize;
  u8 _x = ((x + xsize) - 1) % xsize;
  u8 y_ = (y + 1) % ysize;
  u8 _y = ((y + ysize) - 1) % ysize;

  //Round the clock
  return lifenow[x][_y] +
    lifenow[x_][_y] +
    lifenow[x_][y] +
    lifenow[x_][y_] +
    lifenow[x][y_] +
    lifenow[_x][y_] +
    lifenow[_x][y] +
    lifenow[_x][_y];
}

// pickle / unpickle
u8* op_life_pickle(op_life_t* op, u8* dst) {
  dst = pickle_io(op->xsize, dst);
  dst = pickle_io(op->ysize, dst);
  dst = pickle_io(op->x, dst);
  dst = pickle_io(op->y, dst);
  dst = pickle_io(op->rules, dst);
  return dst;
}

const u8* op_life_unpickle(op_life_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->xsize));
  src = unpickle_io(src, &(op->ysize));
  src = unpickle_io(src, &(op->x));
  src = unpickle_io(src, &(op->y));
  src = unpickle_io(src, &(op->rules));
  return src;
}
