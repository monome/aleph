#include "print_funcs.h"

#include "net_protected.h"
#include "op_life_raw.h"

//-------------------------------------------------
//----- static function declaration
static void op_life_raw_in_next(op_life_raw_t* life, const io_t v);
static void op_life_raw_in_xsize(op_life_raw_t* life, const io_t v);
static void op_life_raw_in_ysize(op_life_raw_t* life, const io_t v);
static void op_life_raw_in_x(op_life_raw_t* life, const io_t v);
static void op_life_raw_in_y(op_life_raw_t* life, const io_t v);
static void op_life_raw_in_write(op_life_raw_t* life, const io_t v);
static void op_life_raw_in_read(op_life_raw_t* life, const io_t v);
static void op_life_raw_in_tog(op_life_raw_t* life, const io_t v);
static void op_life_raw_in_rules(op_life_raw_t* life, const io_t v);

// life
static void life_init(op_life_raw_t *life);
static u8 neighbors(op_life_raw_t *life, u8,u8,u8,u8);

// pickle / unpickle
static u8* op_life_raw_pickle(op_life_raw_t* op, u8* dst);
static const u8* op_life_raw_unpickle(op_life_raw_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_life_raw_in_fn[9] = {
  (op_in_fn)&op_life_raw_in_x,
  (op_in_fn)&op_life_raw_in_y,
  (op_in_fn)&op_life_raw_in_write,
  (op_in_fn)&op_life_raw_in_read,
  (op_in_fn)&op_life_raw_in_tog,
  (op_in_fn)&op_life_raw_in_next,
  (op_in_fn)&op_life_raw_in_xsize,
  (op_in_fn)&op_life_raw_in_ysize,
  (op_in_fn)&op_life_raw_in_rules,
};

static const char* op_life_raw_instring  = "X\0      Y\0      WRITE\0  READ\0   TOG\0    NEXT\0   XSIZE\0  YSIZE\0  RULES\0  ";
static const char* op_life_raw_outstring = "X\0      Y\0      VAL\0    POP\0    ";
static const char* op_life_raw_opstring  = "LIFERAW";


//-------------------------------------------------
//----- external function definitions
void op_life_raw_init(void* mem) {
  op_life_raw_t* life = (op_life_raw_t*)mem;

  //--- monome
  life->super.numInputs = 8;
  life->super.numOutputs = 4;
  life->outs[0] = -1;
  life->outs[1] = -1;
  life->outs[2] = -1;
  life->outs[3] = -1;

  life->super.in_fn = op_life_raw_in_fn;
  life->super.pickle = (op_pickle_fn) (&op_life_raw_pickle);
  life->super.unpickle = (op_unpickle_fn) (&op_life_raw_unpickle);

  life->super.in_val = life->in_val;
  life->super.out = life->outs;
  life->super.opString = op_life_raw_opstring;
  life->super.inString = op_life_raw_instring;
  life->super.outString = op_life_raw_outstring;
  life->super.type = eOpLifeRaw;
  life->super.flags |= (1 << eOpFlagMonomeGrid);
  
  life->in_val[0] = &(life->x);
  life->in_val[1] = &(life->y);
  life->in_val[2] = &(life->write);
  life->in_val[3] = &(life->read);
  life->in_val[4] = &(life->tog);
  life->in_val[5] = &(life->next);
  life->in_val[6] = &(life->xsize);
  life->in_val[7] = &(life->ysize);
  life->in_val[8] = &(life->rules);

  life->next = 0;
  //??? FIXME
  life->xsize = monome_size_x();
  life->ysize = monome_size_y();

  life->x = 0;
  life->y = 0;
  life->write = 0;
  life->read = 0;
  life->tog = 0;
  life->rules = 0;

  life->pop = life->lpop = 0;

  life_init(life);

}

// de-init
void op_life_raw_deinit(void* op) {
}

//-------------------------------------------------
//----- static function definitions

static inline void output_cell (op_life_raw_t* life, u8 x, u8 y, u8 z) {
  net_activate(life, 0, x);
  net_activate(life, 1, y);
  net_activate(life, 2, z);
}

static void op_life_raw_in_next(op_life_raw_t* life, const io_t v) {
  if(v != 0) {
    life->next = 1;
    u8 x, y, count;
    for(x=0; x < life->xsize; x++) {
      for(y=0; y < life->ysize; y++) {
        count = neighbors(life, x, y, life->xsize, life->ysize);

	//Any dead cell with exactly 3 neighors comes alive
        if (count == 3 &&
	    life->lifenow[x][y] == 0 &&
	    life->rules != 2)
          life->lifenext[x][y]=1;
	//Any live cell with < 2 neighbors or > 3 neighbors dies
        else if ((count < 2 || count > 3) &&
		 life->lifenow[x][y] == 1 &&
		 life->rules != 1)
	  life->lifenext[x][y] = 0 ;
	//Any other cell stays the same
	else
	  life->lifenext[x][y] = life->lifenow[x][y];
      }
    }
    u8 pop = 0;
    for(x=0; x < life->xsize; x++) {
      for(y=0; y < life->ysize; y++) {
    	//Draw cells that have changed
	if (life->lifenext[x][y] != life->lifenow[x][y])
	  output_cell(life, x, y, life->lifenext[x][y]);
	life->lifenow[x][y] = life->lifenext[x][y];

	//increment population counter
	pop += life->lifenext[x][y];
      }
    }

    net_activate(life, 3, pop);
  }
  else
    life->next = 0;
}

static void op_life_raw_in_xsize(op_life_raw_t* life, const io_t v) {
  if(v > 16)
    life->xsize = 16;
  if (v < 0)
    life->xsize = 0;
  else
    life->xsize = v;
}

static void op_life_raw_in_ysize(op_life_raw_t* life, const io_t v) {
  if(v > 16)
    life->ysize = 16;
  if (v < 0)
    life->ysize = 0;
  else
    life->ysize = v;
}

static void op_life_raw_in_x(op_life_raw_t* life, const io_t v) {
  if(v > 16)
    life->x = 16;
  if (v < 0)
    life->x = 0;
  else
    life->x = v;
}

static void op_life_raw_in_y(op_life_raw_t* life, const io_t v) {
  if(v > 16)
    life->y = 16;
  if (v < 0)
    life->y = 0;
  else
    life->y = v;
}

static void op_life_raw_in_write(op_life_raw_t* life, const io_t v) {
  if (life->x < life->xsize &&
      life->y < life->ysize) {
    if (v <= 0) {
      life->lifenow[life->x][life->y] = 0;
      life->write = 0;
    }
    else {
      life->lifenow[life->x][life->y] = 1;
      life->write = 1;
    }
    output_cell(life, life->x, life->y, life->lifenow[life->x][life->y]);
  }
}

static void op_life_raw_in_read(op_life_raw_t* life, const io_t v) {
  if (v != 0 &&
      life->x < life->xsize &&
      life->y < life->ysize) {
    life->read = 1;
    output_cell(life, life->x, life->y, life->lifenow[life->x][life->y]);
  }
}

static void op_life_raw_in_tog(op_life_raw_t* life, const io_t v) {
  if (v != 0 &&
      life->x < life->xsize &&
      life->y < life->ysize) {
    life->lifenow[life->x][life->y] = ! life->lifenow[life->x][life->y];
    output_cell(life, life->x, life->y, life->lifenow[life->x][life->y]);
  }
}

static void op_life_raw_in_rules(op_life_raw_t* life, const io_t v) {
  if (v < 1)
    life->rules = 0;
  else if (v > 1)
    life->rules = 2;
  else
    life->rules = v;
}

static void life_init(op_life_raw_t* life) {
  int x, y;
  for(x = 0; x < 16; x++) {
    for(y = 0; y < 16; y++) {
      life->lifenow[x][y] = 0;
      life->lifenext[x][y] = 0;
    }
  }
}

static u8 neighbors(op_life_raw_t *life, u8 x, u8 y, u8 xsize, u8 ysize) {
  u8 x_ = (x + 1) % xsize;
  u8 _x = ((x + xsize) - 1) % xsize;
  u8 y_ = (y + 1) % ysize;
  u8 _y = ((y + ysize) - 1) % ysize;

  //Round the clock
  return life->lifenow[x][_y] +
    life->lifenow[x_][_y] +
    life->lifenow[x_][y] +
    life->lifenow[x_][y_] +
    life->lifenow[x][y_] +
    life->lifenow[_x][y_] +
    life->lifenow[_x][y] +
    life->lifenow[_x][_y];
}

// pickle / unpickle
u8* op_life_raw_pickle(op_life_raw_t* op, u8* dst) {
  dst = pickle_io(op->x, dst);
  dst = pickle_io(op->y, dst);
  dst = pickle_io(op->write, dst);
  dst = pickle_io(op->read, dst);
  dst = pickle_io(op->tog, dst);
  dst = pickle_io(op->next, dst);
  dst = pickle_io(op->xsize, dst);
  dst = pickle_io(op->ysize, dst);
  dst = pickle_io(op->rules, dst);
  int i, j;
  for (i=0; i < 16; i++) {
    for (j=0; j < 16; j++) {
      *dst = op->lifenow[i][j];
      dst++;
    }
  }

  return dst;
}

const u8* op_life_raw_unpickle(op_life_raw_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->x));
  src = unpickle_io(src, &(op->y));
  src = unpickle_io(src, &(op->write));
  src = unpickle_io(src, &(op->read));
  src = unpickle_io(src, &(op->tog));
  src = unpickle_io(src, &(op->next));
  src = unpickle_io(src, &(op->xsize));
  src = unpickle_io(src, &(op->ysize));
  src = unpickle_io(src, &(op->rules));
  int i, j;
  for (i=0; i < 16; i++) {
    for (j=0; i < 16; i++) {
      op->lifenow[i][j] = *src;
      src++;
    }
  }
  return src;
}
