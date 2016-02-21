#include "net_protected.h"
#include "op_mem2d.h"

//-------------------------------------------------
//---- static func declare
static void op_mem2d_in_colsel(op_mem2d_t* mem2d, const io_t v) ;
static void op_mem2d_in_rowsel(op_mem2d_t* mem2d, const io_t v) ;
static void op_mem2d_in_tog(op_mem2d_t* mem2d, const io_t v) ;
static void op_mem2d_in_read(op_mem2d_t* mem2d, const io_t v) ;
static void op_mem2d_in_write(op_mem2d_t* mem2d, const io_t v) ;
static void op_mem2d_in_y(op_mem2d_t* mem2d, const io_t v) ;
static void op_mem2d_in_x(op_mem2d_t* mem2d, const io_t v) ;

// pickle / unpickle
static u8* op_mem2d_pickle(op_mem2d_t* op, u8* dst);
static const u8* op_mem2d_unpickle(op_mem2d_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_mem2d_instring  = "X\0      Y\0      WRITE\0  READ\0   TOG\0    ROWSEL \0COLSEL \0";
static const char* op_mem2d_outstring = "X\0      Y\0      VAL\0    ";
static const char* op_mem2d_opstring = "MEM2D";

static op_in_fn op_mem2d_in_fn[7] = {
  (op_in_fn)&op_mem2d_in_x,
  (op_in_fn)&op_mem2d_in_y,
  (op_in_fn)&op_mem2d_in_write,
  (op_in_fn)&op_mem2d_in_read,
  (op_in_fn)&op_mem2d_in_tog,
  (op_in_fn)&op_mem2d_in_rowsel,
  (op_in_fn)&op_mem2d_in_colsel
};

//-------------------------------------------------
//---- external func define
void op_mem2d_init(void* mem) {
  op_mem2d_t* mem2d = (op_mem2d_t*)mem;
  mem2d->super.numInputs = 7;
  mem2d->super.numOutputs = 3;
  mem2d->outs[0] = -1;
  mem2d->outs[1] = -1;
  mem2d->outs[2] = -1;

  mem2d->super.in_fn = op_mem2d_in_fn;
  mem2d->super.in_val = mem2d->in_val;
  mem2d->super.pickle = (op_pickle_fn) (&op_mem2d_pickle);
  mem2d->super.unpickle = (op_unpickle_fn) (&op_mem2d_unpickle);

  mem2d->super.out = mem2d->outs;
  mem2d->super.opString = op_mem2d_opstring;
  mem2d->super.inString = op_mem2d_instring;
  mem2d->super.outString = op_mem2d_outstring;
  mem2d->super.type = eOpMem2d;

  mem2d->in_val[0] = &(mem2d->x);
  mem2d->in_val[1] = &(mem2d->y);
  mem2d->in_val[2] = &(mem2d->write);
  mem2d->in_val[3] = &(mem2d->read);
  mem2d->in_val[4] = &(mem2d->tog);
  mem2d->in_val[5] = &(mem2d->rowsel);
  mem2d->in_val[6] = &(mem2d->colsel);

  mem2d->x = 0;
  mem2d->y = 0;
  mem2d->write = 0;
  mem2d->read = 0;
  mem2d->tog = 0;
  mem2d->rowsel = 0;
  mem2d->colsel = 0;

}

io_t mem[16][16];

//-------------------------------------------------
//---- static func define

//net_activate(mem2d->outs[0], mem2d->val, mem2d);

static void op_mem2d_in_x(op_mem2d_t* mem2d, const io_t v) {
  if(v > 16)
    mem2d->x = 16;
  if (v < 0)
    mem2d->x = 0;
  else
    mem2d->x = v;
}

static void op_mem2d_in_y(op_mem2d_t* mem2d, const io_t v) {
  if(v > 16)
    mem2d->y = 16;
  if (v < 0)
    mem2d->y = 0;
  else
    mem2d->y = v;
}

static void op_mem2d_in_write(op_mem2d_t* mem2d, const io_t v) {
  mem2d->write = mem[mem2d->x][mem2d->y] = v;
}

static void op_mem2d_in_read(op_mem2d_t* mem2d, const io_t v) {
  if (v > 0) {
    mem2d->read = 1;
    net_activate(mem2d->outs[0], mem2d->x, mem2d);
    net_activate(mem2d->outs[1], mem2d->y, mem2d);
    net_activate(mem2d->outs[2], mem[mem2d->x][mem2d->y], mem2d);
  }
  else
    mem2d->read = 0;
}

static void op_mem2d_in_tog(op_mem2d_t* mem2d, const io_t v) {
  if (v > 0) {
    mem2d->tog = 1;
    mem2d->write = ! mem[mem2d->x][mem2d->y];
  }
  else
    mem2d->tog = 0;
}

static void op_mem2d_in_rowsel(op_mem2d_t* mem2d, const io_t v) {
  if (v >= 0) {
    mem2d->rowsel = v;
    net_activate(mem2d->outs[1], mem2d->y, mem2d);
    u8 x;
    for (x=0; x < 16; x++) {
      if (x == v) {
	mem[x][mem2d->y] = 1;
	net_activate(mem2d->outs[0], x, mem2d);
	net_activate(mem2d->outs[2], mem[x][mem2d->y], mem2d);
      } else if ( mem[x][mem2d->y] != 0) {
	mem[x][mem2d->y] = 0;
	net_activate(mem2d->outs[0], x, mem2d);
	net_activate(mem2d->outs[2], mem[x][mem2d->y], mem2d);
      }
    }
  }
}

static void op_mem2d_in_colsel(op_mem2d_t* mem2d, const io_t v) {
  if (v >= 0) {
    mem2d->colsel = v;
    net_activate(mem2d->outs[0], mem2d->x, mem2d);
    u8 y;
    for (y=0; y < 16; y++) {
      if (y == v) {
	mem[y][mem2d->x] = 1;
	net_activate(mem2d->outs[1], y, mem2d);
	net_activate(mem2d->outs[2], mem[mem2d->x][y], mem2d);
      } else if ( mem[mem2d->x][y] != 0) {
	mem[mem2d->x][y] = 0;
	net_activate(mem2d->outs[1], y, mem2d);
	net_activate(mem2d->outs[2], mem[mem2d->x][y], mem2d);
      }
    }
  }
}



// pickle / unpickle
u8* op_mem2d_pickle(op_mem2d_t* op, u8* dst) {
  dst = pickle_io(op->x, dst);
  dst = pickle_io(op->y, dst);
  dst = pickle_io(op->write, dst);
  dst = pickle_io(op->read, dst);
  dst = pickle_io(op->tog, dst);
  dst = pickle_io(op->rowsel, dst);
  dst = pickle_io(op->colsel, dst);
  return dst;
}

const u8* op_mem2d_unpickle(op_mem2d_t* op, const u8* src) {
  src = unpickle_io(src, &(op->x));
  src = unpickle_io(src, &(op->y));
  src = unpickle_io(src, &(op->write));
  src = unpickle_io(src, &(op->read));
  src = unpickle_io(src, &(op->tog));
  src = unpickle_io(src, &(op->rowsel));
  src = unpickle_io(src, &(op->colsel));
  return src;
}
