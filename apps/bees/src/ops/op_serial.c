// asf


#ifdef BEEKEEP
#else
#include "usart.h"
#include "aleph_board.h"
#endif

// avr32 lib
// bees
#include "net_protected.h"
#include "op_is.h"
#include "ser.h"

//-------------------------------------------------
//----- static function declaration
static void op_serial_in_a(op_serial_t* is, const io_t v);
static void op_serial_in_b(op_serial_t* is, const io_t v);


// pickle / unpickle
static u8* op_serial_pickle(op_serial_t* op, u8* dst);
static const u8* op_serial_unpickle(op_serial_t* op, const u8* src);


// transmit buffer
//static char txbuf[4] = { '0','0','\0','\0' };
// transmit function
static inline void op_serial_tx(op_serial_t* op) {
  // FIXME: hardcoded conversion of inputs to 2 bytes for now
  /* txbuf[0] = (char)op_from_int(op->a); */
  /* txbuf[1] = (char)op_from_int(op->b);   */
  /* usart_write_line(DEV_USART, txbuf); */



#ifdef BEEKEEP
#else
  /// ok, all the bytes? really?
  /* usart_putchar( DEV_USART, (char)op_from_int(op->a) ); */
  /* usart_putchar( DEV_USART, (char)op_from_int(op->b) ); */
  usart_putchar( DEV_USART, (char)((op_from_int(op->a) & 0xff00) >> 8) );
  usart_putchar( DEV_USART, (char)((op_from_int(op->a) & 0xff)) );
  usart_putchar( DEV_USART, (char)((op_from_int(op->b) & 0xff00) >> 8) );
  usart_putchar( DEV_USART, (char)((op_from_int(op->b) & 0xff)) );
#endif
}

//-------------------------------------------------
//----- static vars
static op_in_fn op_serial_in_fn[2] = {
  (op_in_fn)&op_serial_in_a,
  (op_in_fn)&op_serial_in_b,
};

static const char* op_serial_instring  = "ADDR\0   DATA\0   ";
static const char* op_serial_opstring  = "SERIAL";

//-------------------------------------------------
//----- external function definitions
void op_serial_init(void* mem) {
  op_serial_t* op = (op_serial_t*)mem;
  op->super.numInputs = 2;
  op->super.numOutputs = 0;

  op->super.in_fn = op_serial_in_fn;
  op->super.pickle = (op_pickle_fn) (&op_serial_pickle);
  op->super.unpickle = (op_unpickle_fn) (&op_serial_unpickle);

  op->super.in_val = op->in_val;
  op->super.out = op->outs;
  op->super.opString = op_serial_opstring;
  op->super.inString = op_serial_instring;

  op->super.type = eOpSerial;  

  op->in_val[0] = &(op->a);
  op->in_val[1] = &(op->b);

  op->a = 0;
  op->b = 0;
}

void op_serial_deinit(void* op) {
  // nothing to do
}

//-------------------------------------------------
//----- static function definitions
// set operand A
static void op_serial_in_a(op_serial_t* op, const io_t v) {
  op->a = v;
}

// set operand B
static void op_serial_in_b(op_serial_t* op, const io_t v) {
  op->b = v;
#ifdef BEEKEEP
#else
  serial_outVal(op->a, op->b);
#endif
}

/* void op_serial_out (op_serial_t* op, const io_t v) { */
/*   net_activate(op, 0, v); */
/* } */

// pickle / unpickle
u8* op_serial_pickle(op_serial_t* op, u8* dst) {
  dst = pickle_io(op->a, dst);
  dst = pickle_io(op->b, dst);
  return dst;
}

const u8* op_serial_unpickle(op_serial_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->a));
  src = unpickle_io(src, &(op->b));
  return src;
}
