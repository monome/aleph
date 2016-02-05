#include "print_funcs.h"

#include "aleph_board.h"
#include "usart.h"

#include "bfin.h"
#include "events.h"
#include "event_types.h"
#include "serial.h"
#include "delay.h"

//----------------------------
//----- extern vatrs
volatile u8 serial_buffer[SERIAL_BUFFER_SIZE];

static u16 serial_write_pos = 0;
static u16 last_serial_write_pos = 0;
static event_t e;

void serial_store() {
  // process_serial_t serial_decode = &serial_decode_dummy;
  int c;

  //buffer, try to grab more than one byte if available
  while(usart_read_char(DEV_USART,&c) == USART_SUCCESS) {

      // TEST LIB LOOPBACK
      // usart_putchar(DBG_USART,c);
      // print_dbg_char(c);
    last_serial_write_pos = serial_write_pos;
      serial_buffer[serial_write_pos] = c;
      serial_write_pos++;
      if(serial_write_pos == SERIAL_BUFFER_SIZE) serial_write_pos = 0;
      if (serial_buffer[last_serial_write_pos] == 0x13) {
	e.type = kEventSerial;
	e.data = serial_write_pos;
	event_post(&e);
      }
  }
}

