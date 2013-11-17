#include "print_funcs.h"

#include "aleph_board.h"
#include "usart.h"

#include "bfin.h"
#include "serial.h"


u8 serial_buffer[SERIAL_BUFFER_SIZE];
u8 serial_read_pos = 0;
u8 serial_write_pos = 0;
u8 serial_new_bytes = 0;

typedef enum {
	eComNull,
	eComSetDebug,
	eComReqNumParams,
	eComReqParamInfo,
	eComGetParamVal,
	eComSetParamVal,
	eComNumCommands
} eSerialCommands;

static void serial_decode_dummy(void) { return; }

static const process_serial_t serialFuncs[eComNumCommands] = {
  &serial_decode_dummy,
  &serial_decode_dummy,
  &com_req_num_params,
  &com_req_param_info,
  &com_get_param,
  &com_set_param
};

process_serial_t serial_decode = &serial_decode_dummy;

void serial_process() {
	int c;
 	//buffer, try to grab more than one byte if available
 	while(usart_read_char(FTDI_USART,&c) == USART_SUCCESS) {
		serial_buffer[serial_write_pos] = c;

		// FIXME: overflow control
		// if(serial_write_pos > SERIAL_BUFFER_SIZE) ;;

		// FIXME: implement proper framing, ie: http://eli.thegreenplace.net/2009/08/12/framing-in-serial-communications/
		// right now a data '0' will mess shit up!
		// check for null, indicating complete packet
		if(c == 0) {
			if(serial_buffer[0] >= eComNumCommands) {		// check for garbage command
				print_dbg("bad serial command.");
				serial_write_pos = 0;
			} else {
				serial_decode = serialFuncs[serial_buffer[0]];			// first byte is index
				(*serial_decode)();			// do the thing according to command index, serial_write_pos still has packet length
				serial_write_pos = 0;		// reset for next packet
			}
		} else serial_write_pos++;
	}
}

void com_req_num_params(void) {
	print_dbg("req_num_params:");
	print_dbg_ulong(serial_buffer[0]);
	print_dbg_ulong(serial_buffer[1]);
}

void com_req_param_info(void) {
	print_dbg("req_param_info:");
	print_dbg_ulong(serial_buffer[0]);
	print_dbg_ulong(serial_buffer[1]);
}

void com_get_param(void) {
	// probably should check length of packet first!
	if(serial_write_pos<2)
		print_dbg("bad serial data.");
	else {
		print_dbg("get_param:");
		print_dbg_ulong(serial_buffer[0]);
		print_dbg_ulong(serial_buffer[1]);
		print_dbg_ulong(serial_buffer[2]);
	}
}

void com_set_param(void) {
	if(serial_write_pos<5)
		print_dbg("bad serial data.");
	else {
		print_dbg("set_param:");
		print_dbg_ulong(serial_buffer[0]);
		print_dbg_ulong(serial_buffer[1]);
		print_dbg_ulong(serial_buffer[2]);
		print_dbg_ulong(serial_buffer[3]);
		print_dbg_ulong(serial_buffer[4]);
		print_dbg_ulong(serial_buffer[5]);
	}
}

