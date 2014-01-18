#include "print_funcs.h"

//#include "aleph_board.h"
//#include "usart.h"

#include "bfin.h"
#include "events.h"
#include "event_types.h"
#include "serial.h"

// SERIAL DESCRIPTION:
// escape character is 27 (ESC)
// unit separator is 31 (for end-of variable-length params)
// end-of-packet is 0 (NULL)

//----------------------------
//----- extern vatrs
volatile u8 serial_buffer[SERIAL_BUFFER_SIZE];


//----------------------------
//-- static vars / types

typedef enum {
  eComNull,
  eComSetDebug,
  eComReqNumParams,
  eComReqParamInfo,
  eComGetParamVal,
  eComSetParamVal,
  eComNumCommands
} eSerialCommands;

static u16 serial_read_pos = 0;
static u16 serial_write_pos = 0;
static u8 escape = 0;
static event_t e;

//--------------------------------
//---- static funcs
static void com_req_num_params(u16);
static void com_req_param_info(u16);
static void com_get_param(u16);
static void com_set_param(u16);

static void serial_decode_dummy(u16 pos) { return; }

static const process_serial_t serialFuncs[eComNumCommands] = {
  &serial_decode_dummy,
  &serial_decode_dummy,
  &com_req_num_params,
  &com_req_param_info,
  &com_get_param,
  &com_set_param
};

void serial_send_start(u8 index) {
  //  usart_putchar(DBG_USART,index);
}

void serial_send_byte(u8 data) {
#if 1
#else
  // FIXME this is slow
  if(data == 0 || data == 27 || data == 31) {
    usart_putchar(DBG_USART,27);
  }
  usart_putchar(DBG_USART,data);
#endif
}

void serial_send_long(u32 data) {
#if 1
#else
  serial_send_byte((u8)data);
  serial_send_byte((u8)data>>8);
  serial_send_byte((u8)data>>16);
  serial_send_byte((u8)data>>24);
#endif
}

void serial_send_separator(void) {
#if 1
#else
  usart_putchar(DBG_USART,31);
#endif
}

void serial_send_end(void) {
#if 1
#else
  usart_putchar(DBG_USART,0);
#endif
}

void serial_process() {
#if 1
#else
  process_serial_t serial_decode = &serial_decode_dummy;
  int c;

  //buffer, try to grab more than one byte if available
  while(usart_read_char(FTDI_USART,&c) == USART_SUCCESS) {
    // DONE: implement proper framing, ie: http://eli.thegreenplace.net/2009/08/12/framing-in-serial-communications/
    // code 27 is escape
    if(c == 27 && escape == 0) escape = 1;
    // check for null, indicating complete packet
    else if(c == 0 && escape == 0) {
      if(serial_buffer[0] >= eComNumCommands) {		// check for garbage command
	print_dbg("bad serial command.");
	serial_write_pos++;
      } else {
	serial_decode = serialFuncs[serial_buffer[serial_read_pos]];			// first byte is index
	(*serial_decode)(serial_read_pos);			// do the thing according to command index
	serial_write_pos++;
	if(serial_write_pos == SERIAL_BUFFER_SIZE) serial_write_pos = 0;	// wrap
	serial_read_pos = serial_write_pos;			// start of next packet
      }
      // normal data copy
    } else {
      serial_buffer[serial_write_pos] = c;
      serial_write_pos++;
      if(serial_write_pos == SERIAL_BUFFER_SIZE) serial_write_pos = 0;
      escape = 0;
    }
  }
#endif
}

// from handler
void serial_param_num(s32 data) {
#if 1
#else
  u32 num;
  bfin_get_num_params(&num);
  serial_send_start(2);
  serial_send_byte(num);
  serial_send_end();
#endif
}

void serial_param_info(s32 data) {
#if 1
#else
    // TODO check out of bounds index
  static ParamDesc p;
  u8 idx = serial_buffer[data+1];
  u8 c = 1, n = 0;
  bfin_get_param_desc(idx, &p);
  serial_send_start(3);
  serial_send_byte(idx);
  while(c != 0 && n < PARAM_LABEL_LEN) {
    c = p.label[n];
    serial_send_byte(c);
    n++;
  }
  serial_send_separator();
  serial_send_end();
#endif
}

void serial_param_get(s32 data) {
#if 1
#else
  u32 val;
  u8 idx = serial_buffer[data+1];

  // index check for bounds?
  val = bfin_get_param(idx);
  // print_dbg("\nvalue: ");
  // print_dbg_ulong(val);
  // print_dbg(" : ");
  // print_dbg_ulong((u8)(val>>24 & 0xff));
  // print_dbg(" ");
  // print_dbg_ulong((u8)(val>>16 & 0xff));
  // print_dbg(" ");
  // print_dbg_ulong((u8)(val>>8 & 0xff));
  // print_dbg(" ");
  // print_dbg_ulong((u8)val & 0xff);

  serial_send_start(4);
  serial_send_byte(idx);
  serial_send_byte((u8)(val>>24 & 0xff));
  serial_send_byte((u8)(val>>16 & 0xff));
  serial_send_byte((u8)(val>>8 & 0xff));
  serial_send_byte((u8)val & 0xff);
  serial_send_end();
#endif
}

void serial_param_set(s32 data) {
#if 1
#else
  u8 idx = serial_buffer[data+1];
  s32 val = (serial_buffer[data+2]<<24) + (serial_buffer[data+3]<<16) + (serial_buffer[data+4]<<8) + (serial_buffer[data+5]);
  bfin_set_param(idx, val);
#endif
}





// packet parsing called from serial_process according to index

void com_req_num_params(u16 pos) {
#if 1
#else
  e.type = kEventSerialParamNum;
  e.data = pos; 
  event_post(&e);

  // print_dbg("req_num_params:");
  // print_dbg_ulong(serial_buffer[pos]);
#endif
}

void com_req_param_info(u16 pos) {
#if 1
#else
  // print_dbg("req_param_info:");
  // print_dbg_ulong(serial_buffer[pos]);
  // print_dbg_ulong(serial_buffer[pos+1]);
  e.type = kEventSerialParamInfo;
  e.data = pos; 
  event_post(&e);	
#endif
}

void com_get_param(u16 pos) {
#if 1
#else
  // print_dbg("get_param:");
  // print_dbg_ulong(serial_buffer[pos]);
  // print_dbg_ulong(serial_buffer[pos+1]);
  e.type = kEventSerialParamGet;
  e.data = pos; 
  event_post(&e);
#endif
}

void com_set_param(u16 pos) {
#if 1
#else
  // print_dbg("set_param:");
  // print_dbg_ulong(serial_buffer[pos]);
  // print_dbg_ulong(serial_buffer[pos+1]);
  // print_dbg_ulong(serial_buffer[pos+2]);
  // print_dbg_ulong(serial_buffer[pos+3]);
  // print_dbg_ulong(serial_buffer[pos+4]);
  // print_dbg_ulong(serial_buffer[pos+5]);
  e.type = kEventSerialParamSet;
  e.data = pos; 
  event_post(&e);
#endif
}

