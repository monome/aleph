#include "net_protected.h"
#include "op_mem0d.h"

//-------------------------------------------------
//---- static func declare
static void op_mem0d_in_tog(op_mem0d_t* mem0d, const io_t v) ;
static void op_mem0d_in_read(op_mem0d_t* mem0d, const io_t v) ;
static void op_mem0d_in_write(op_mem0d_t* mem0d, const io_t v) ;

// pickle / unpickle
static u8* op_mem0d_pickle(op_mem0d_t* op, u8* dst);
static const u8* op_mem0d_unpickle(op_mem0d_t* op, const u8* src);

//-------------------------------------------------
//---- static vars
static const char* op_mem0d_instring  = "WRITE\0  READ\0   TOG\0    ";
// Note: commented lines in this file are to enable using this 2D array
// to represent 1D data. Disabled but leaving in place for now...

static const char* op_mem0d_outstring = "VAL\0    ";
static const char* op_mem0d_opstring = "MEM0D";

static op_in_fn op_mem0d_in_fn[3] = {
  (op_in_fn)&op_mem0d_in_write,
  (op_in_fn)&op_mem0d_in_read,
  (op_in_fn)&op_mem0d_in_tog
};

//-------------------------------------------------
//---- external func define
void op_mem0d_init(void* mem) {
  op_mem0d_t* mem0d = (op_mem0d_t*)mem;
  mem0d->super.numInputs = 3;
  mem0d->super.numOutputs = 1;
  mem0d->outs[0] = -1;

  mem0d->super.in_fn = op_mem0d_in_fn;
  mem0d->super.in_val = mem0d->in_val;
  mem0d->super.pickle = (op_pickle_fn) (&op_mem0d_pickle);
  mem0d->super.unpickle = (op_unpickle_fn) (&op_mem0d_unpickle);

  mem0d->super.out = mem0d->outs;
  mem0d->super.opString = op_mem0d_opstring;
  mem0d->super.inString = op_mem0d_instring;
  mem0d->super.outString = op_mem0d_outstring;
  mem0d->super.type = eOpMem0d;

  mem0d->in_val[0] = &(mem0d->write);
  mem0d->in_val[1] = &(mem0d->read);
  mem0d->in_val[2] = &(mem0d->tog);

  mem0d->write = 0;
  mem0d->read = 0;
  mem0d->tog = 0;
  mem0d->rowsel = 0;
  mem0d->colsel = 0;

}

//-------------------------------------------------
//---- static func define

static void op_mem0d_in_write(op_mem0d_t* mem0d, const io_t v) {
  mem0d->write = v;
}

static void op_mem0d_in_read(op_mem0d_t* mem0d, const io_t v) {
  net_activate(mem0d->outs[0], mem0d->write, mem0d);
}

static void op_mem0d_in_tog(op_mem0d_t* mem0d, const io_t v) {
  if (v > 0) {
    mem0d->tog = 1;
    mem0d->write = ! mem0d->write;
  }
  else {
    mem0d->tog = 0;
  }
}

// pickle / unpickle
u8* op_mem0d_pickle(op_mem0d_t* op, u8* dst) {
  dst = pickle_io(op->write, dst);
  dst = pickle_io(op->read, dst);
  dst = pickle_io(op->tog, dst);
  return dst;
}

const u8* op_mem0d_unpickle(op_mem0d_t* op, const u8* src) {
  src = unpickle_io(src, &(op->write));
  src = unpickle_io(src, &(op->read));
  src = unpickle_io(src, &(op->tog));
  return src;
}
