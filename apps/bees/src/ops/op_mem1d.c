#include "net_protected.h"
#include "op_mem1d.h"

//-------------------------------------------------
//---- static func declare
static void op_mem1d_in_tog(op_mem1d_t* mem1d, const io_t v) ;
static void op_mem1d_in_read(op_mem1d_t* mem1d, const io_t v) ;
static void op_mem1d_in_write(op_mem1d_t* mem1d, const io_t v) ;
static void op_mem1d_in_n(op_mem1d_t* mem1d, const io_t v) ;

// pickle / unpickle
static u8* op_mem1d_pickle(op_mem1d_t* op, u8* dst);
static const u8* op_mem1d_unpickle(op_mem1d_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_mem1d_instring  = "N\0      WRITE\0  READ\0   TOG\0    ";
// Note: commented lines in this file are to enable using this 2D array
// to represent 1D data. Disabled but leaving in place for now...

static const char* op_mem1d_outstring = "N\0      VAL\0    ";
static const char* op_mem1d_opstring = "MEM1D";

static op_in_fn op_mem1d_in_fn[4] = {
  (op_in_fn)&op_mem1d_in_n,
  (op_in_fn)&op_mem1d_in_write,
  (op_in_fn)&op_mem1d_in_read,
  (op_in_fn)&op_mem1d_in_tog
};

//-------------------------------------------------
//---- external func define
void op_mem1d_init(void* mem) {
  op_mem1d_t* mem1d = (op_mem1d_t*)mem;
  mem1d->super.numInputs = 4;
  mem1d->super.numOutputs = 2;
  mem1d->outs[0] = -1;
  mem1d->outs[1] = -1;

  mem1d->super.in_fn = op_mem1d_in_fn;
  mem1d->super.in_val = mem1d->in_val;
  mem1d->super.pickle = (op_pickle_fn) (&op_mem1d_pickle);
  mem1d->super.unpickle = (op_unpickle_fn) (&op_mem1d_unpickle);

  mem1d->super.out = mem1d->outs;
  mem1d->super.opString = op_mem1d_opstring;
  mem1d->super.inString = op_mem1d_instring;
  mem1d->super.outString = op_mem1d_outstring;
  mem1d->super.type = eOpMem1d;

  mem1d->in_val[0] = &(mem1d->n);
  mem1d->in_val[1] = &(mem1d->write);
  mem1d->in_val[2] = &(mem1d->read);
  mem1d->in_val[3] = &(mem1d->tog);

  mem1d->n = 0;
  mem1d->write = 0;
  mem1d->read = 0;
  mem1d->tog = 0;
  mem1d->rowsel = 0;
  mem1d->colsel = 0;

}

io_t globalMem1d[16];

//-------------------------------------------------
//---- static func define

static void op_mem1d_in_n(op_mem1d_t* mem1d, const io_t v) {
  if(v > 16)
    mem1d->n = 16;
  if (v < 0)
    mem1d->n = 0;
  else
    mem1d->n = v;
}

static void op_mem1d_in_write(op_mem1d_t* mem1d, const io_t v) {
  mem1d->write = globalMem1d[mem1d->n] = v;
}

static void op_mem1d_in_read(op_mem1d_t* mem1d, const io_t v) {
  net_activate(mem1d->outs[0], mem1d->n, mem1d);
  net_activate(mem1d->outs[1], globalMem1d[mem1d->n], mem1d);
}

static void op_mem1d_in_tog(op_mem1d_t* mem1d, const io_t v) {
  if (v > 0) {
    mem1d->tog = 1;
    mem1d->write = 1;
    globalMem1d[mem1d->n] = ! globalMem1d[mem1d->n];
  }
  else {
    mem1d->tog = 0;
    mem1d->write = 0;
  }
}

// pickle / unpickle
u8* op_mem1d_pickle(op_mem1d_t* op, u8* dst) {
  dst = pickle_io(op->n, dst);
  dst = pickle_io(op->write, dst);
  dst = pickle_io(op->read, dst);
  dst = pickle_io(op->tog, dst);
  return dst;
}

const u8* op_mem1d_unpickle(op_mem1d_t* op, const u8* src) {
  src = unpickle_io(src, &(op->n));
  src = unpickle_io(src, &(op->write));
  src = unpickle_io(src, &(op->read));
  src = unpickle_io(src, &(op->tog));
  return src;
}
