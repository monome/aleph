#include "net_protected.h"
#include "op_mem1d.h"

//-------------------------------------------------
//---- static func declare
/* static void op_mem1d_in_colsel(op_mem1d_t* mem1d, const io_t v) ; */
/* static void op_mem1d_in_rowsel(op_mem1d_t* mem1d, const io_t v) ; */
static void op_mem1d_in_tog(op_mem1d_t* mem1d, const io_t v) ;
static void op_mem1d_in_read(op_mem1d_t* mem1d, const io_t v) ;
static void op_mem1d_in_write(op_mem1d_t* mem1d, const io_t v) ;
static void op_mem1d_in_y(op_mem1d_t* mem1d, const io_t v) ;
static void op_mem1d_in_x(op_mem1d_t* mem1d, const io_t v) ;

// pickle / unpickle
static u8* op_mem1d_pickle(op_mem1d_t* op, u8* dst);
static const u8* op_mem1d_unpickle(op_mem1d_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_mem1d_instring  = "X\0      Y\0      WRITE\0  READ\0   TOG\0    ";
// Note: commented lines in this file are to enable using this 2D array
// to represent 1D data. Disabled but leaving in place for now...
/* static const char* op_mem1d_instring  = "X\0      Y\0      WRITE\0  READ\0   TOG\0    ROWSEL \0COLSEL \0"; */

static const char* op_mem1d_outstring = "X\0      Y\0      VAL\0    ";
static const char* op_mem1d_opstring = "MEM1D";

static op_in_fn op_mem1d_in_fn[5] = {
  (op_in_fn)&op_mem1d_in_x,
  (op_in_fn)&op_mem1d_in_y,
  (op_in_fn)&op_mem1d_in_write,
  (op_in_fn)&op_mem1d_in_read,
  (op_in_fn)&op_mem1d_in_tog
  /* (op_in_fn)&op_mem1d_in_rowsel, */
  /* (op_in_fn)&op_mem1d_in_colsel */
};

//-------------------------------------------------
//---- external func define
void op_mem1d_init(void* mem) {
  op_mem1d_t* mem1d = (op_mem1d_t*)mem;
  mem1d->super.numInputs = 5;
  mem1d->super.numOutputs = 3;
  mem1d->outs[0] = -1;
  mem1d->outs[1] = -1;
  mem1d->outs[2] = -1;

  mem1d->super.in_fn = op_mem1d_in_fn;
  mem1d->super.in_val = mem1d->in_val;
  mem1d->super.pickle = (op_pickle_fn) (&op_mem1d_pickle);
  mem1d->super.unpickle = (op_unpickle_fn) (&op_mem1d_unpickle);

  mem1d->super.out = mem1d->outs;
  mem1d->super.opString = op_mem1d_opstring;
  mem1d->super.inString = op_mem1d_instring;
  mem1d->super.outString = op_mem1d_outstring;
  mem1d->super.type = eOpMem1d;

  mem1d->in_val[0] = &(mem1d->x);
  mem1d->in_val[1] = &(mem1d->y);
  mem1d->in_val[2] = &(mem1d->write);
  mem1d->in_val[3] = &(mem1d->read);
  mem1d->in_val[4] = &(mem1d->tog);
  /* mem1d->in_val[5] = &(mem1d->rowsel); */
  /* mem1d->in_val[6] = &(mem1d->colsel); */

  mem1d->x = 0;
  mem1d->y = 0;
  mem1d->write = 0;
  mem1d->read = 0;
  mem1d->tog = 0;
  mem1d->rowsel = 0;
  mem1d->colsel = 0;

}

io_t globalMem1d[16][16];

//-------------------------------------------------
//---- static func define

//net_activate(mem1d->outs[0], mem1d->val, mem1d);

static void op_mem1d_in_x(op_mem1d_t* mem1d, const io_t v) {
  if(v > 16)
    mem1d->x = 16;
  if (v < 0)
    mem1d->x = 0;
  else
    mem1d->x = v;
}

static void op_mem1d_in_y(op_mem1d_t* mem1d, const io_t v) {
  if(v > 16)
    mem1d->y = 16;
  if (v < 0)
    mem1d->y = 0;
  else
    mem1d->y = v;
}

static void op_mem1d_in_write(op_mem1d_t* mem1d, const io_t v) {
  mem1d->write = globalMem1d[mem1d->x][mem1d->y] = v;
}

static void op_mem1d_in_read(op_mem1d_t* mem1d, const io_t v) {
  net_activate(mem1d->outs[0], mem1d->x, mem1d);
  net_activate(mem1d->outs[1], mem1d->y, mem1d);
  net_activate(mem1d->outs[2], globalMem1d[mem1d->x][mem1d->y], mem1d);
}

static void op_mem1d_in_tog(op_mem1d_t* mem1d, const io_t v) {
  if (v > 0) {
    mem1d->tog = 1;
    mem1d->write = 1;
    globalMem1d[mem1d->x][mem1d->y] = ! globalMem1d[mem1d->x][mem1d->y];
  }
  else {
    mem1d->tog = 0;
    mem1d->write = 0;
  }
}

/* static void op_mem1d_in_rowsel(op_mem1d_t* mem1d, const io_t v) { */
/*   net_activate(mem1d->outs[1], mem1d->y, mem1d); */
/*   u8 x; */
/*   for (x=0; x < 16; x++) { */
/*     if (x == mem1d->x) { */
/*       mem[x][mem1d->y] = 1; */
/*       net_activate(mem1d->outs[0], x, mem1d); */
/*       net_activate(mem1d->outs[2], mem[x][mem1d->y], mem1d); */
/*     } else if ( mem[x][mem1d->y] != 0) { */
/*       mem[x][mem1d->y] = 0; */
/*       net_activate(mem1d->outs[0], x, mem1d); */
/*       net_activate(mem1d->outs[2], mem[x][mem1d->y], mem1d); */
/*     } */
/*   } */
/* } */

/* static void op_mem1d_in_colsel(op_mem1d_t* mem1d, const io_t v) { */
/*   net_activate(mem1d->outs[0], mem1d->x, mem1d); */
/*   u8 y; */
/*   for (y=0; y < 16; y++) { */
/*     if (y == mem1d->y) { */
/*       mem[mem1d->x][y] = 1; */
/*       net_activate(mem1d->outs[1], y, mem1d); */
/*       net_activate(mem1d->outs[2], mem[mem1d->x][y], mem1d); */
/*     } else if ( mem[mem1d->x][y] != 0) { */
/*       mem[mem1d->x][y] = 0; */
/*       net_activate(mem1d->outs[1], y, mem1d); */
/*       net_activate(mem1d->outs[2], mem[mem1d->x][y], mem1d); */
/*     } */
/*   } */
/* } */


// pickle / unpickle
u8* op_mem1d_pickle(op_mem1d_t* op, u8* dst) {
  dst = pickle_io(op->x, dst);
  dst = pickle_io(op->y, dst);
  dst = pickle_io(op->write, dst);
  dst = pickle_io(op->read, dst);
  dst = pickle_io(op->tog, dst);
  /* dst = pickle_io(op->rowsel, dst); */
  /* dst = pickle_io(op->colsel, dst); */
  return dst;
}

const u8* op_mem1d_unpickle(op_mem1d_t* op, const u8* src) {
  src = unpickle_io(src, &(op->x));
  src = unpickle_io(src, &(op->y));
  src = unpickle_io(src, &(op->write));
  src = unpickle_io(src, &(op->read));
  src = unpickle_io(src, &(op->tog));
  /* src = unpickle_io(src, &(op->rowsel)); */
  /* src = unpickle_io(src, &(op->colsel)); */
  return src;
}
