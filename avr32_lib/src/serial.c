#include "print_funcs.h"

#include "aleph_board.h"
#include "usart.h"

#include "events.h"
#include "event_types.h"
#include "serial.h"

// SERIAL DESCRIPTION:
// escape character is 27 (ESC)
// unit separator is 31 (for end-of variable-length params)
// end-of-packet is 0 (NULL)


u8 serial_buffer[SERIAL_BUFFER_SIZE];
u16 serial_read_pos = 0;
u16 serial_write_pos = 0;

u8 escape = 0;

typedef enum {
	eComNull,
	eComSetDebug,
	eComReqNumParams,
	eComReqParamInfo,
	eComGetParamVal,
	eComSetParamVal,
	eComNumCommands
} eSerialCommands;

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
	usart_putchar(DBG_USART,index);
}

void serial_send_byte(u8 data) {
	// FIXME this is slow
	if(data == 0 || data == 27 || data == 31)
		usart_putchar(DBG_USART,27);
	usart_putchar(DBG_USART,data);
}

void serial_send_long(u32 data) {
	serial_send_byte((u8)data);
	serial_send_byte((u8)data>>8);
	serial_send_byte((u8)data>>16);
	serial_send_byte((u8)data>>24);
}

void serial_send_separator(void) {
	usart_putchar(DBG_USART,31);
}

void serial_send_end(void) {
	usart_putchar(DBG_USART,0);
}

void serial_process() {
	process_serial_t serial_decode = &serial_decode_dummy;
	int c;

 	//buffer, try to grab more than one byte if available
 	while(usart_read_char(FTDI_USART,&c) == USART_SUCCESS) {
		// FIXME: overflow control
		// if(serial_write_pos > SERIAL_BUFFER_SIZE) ;;

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
}

void com_req_num_params(u16 pos) {
	static event_t e;   
	e.type = kEventSerialParamNum;
	e.data = pos; 
	event_post(&e);

	// print_dbg("req_num_params:");
	// print_dbg_ulong(serial_buffer[pos]);
}

void com_req_param_info(u16 pos) {
	// print_dbg("req_param_info:");
	// print_dbg_ulong(serial_buffer[pos]);
	// print_dbg_ulong(serial_buffer[pos+1]);
	static event_t e;   
	e.type = kEventSerialParamInfo;
	e.data = pos; 
	event_post(&e);	
}

void com_get_param(u16 pos) {
	// print_dbg("get_param:");
	// print_dbg_ulong(serial_buffer[pos]);
	// print_dbg_ulong(serial_buffer[pos+1]);
	static event_t e;   
	e.type = kEventSerialParamGet;
	e.data = pos; 
	event_post(&e);
}

void com_set_param(u16 pos) {
	// print_dbg("set_param:");
	// print_dbg_ulong(serial_buffer[pos]);
	// print_dbg_ulong(serial_buffer[pos+1]);
	// print_dbg_ulong(serial_buffer[pos+2]);
	// print_dbg_ulong(serial_buffer[pos+3]);
	// print_dbg_ulong(serial_buffer[pos+4]);
	// print_dbg_ulong(serial_buffer[pos+5]);
	static event_t e;   
	e.type = kEventSerialParamSet;
	e.data = pos; 
	event_post(&e);
}

