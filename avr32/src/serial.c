#include "print_funcs.h"

#include "conf_board.h"
#include "usart.h"

#include "bfin.h"
#include "events.h"
#include "serial.h"

//----------------------------
//----- extern vatrs
volatile u8 serial_buffer[SERIAL_BUFFER_SIZE];

static u16 serial_write_pos = 0;
static event_t e;

void serial_send_start(u8 index) { usart_putchar(DBG_USART, index); }

void serial_send_byte(u8 data) {
    // FIXME this is slow
    if (data == 0 || data == 27 || data == 31) { usart_putchar(DBG_USART, 27); }
    usart_putchar(DBG_USART, data);
}

void serial_send_long(u32 data) {
    serial_send_byte((u8)data);
    serial_send_byte((u8)data >> 8);
    serial_send_byte((u8)data >> 16);
    serial_send_byte((u8)data >> 24);
}

void serial_send_separator(void) { usart_putchar(DBG_USART, 31); }

void serial_send_end(void) { usart_putchar(DBG_USART, 0); }

void serial_store() {
    // process_serial_t serial_decode = &serial_decode_dummy;
    int c;

    // buffer, try to grab more than one byte if available
    while (usart_read_char(DEV_USART, &c) == USART_SUCCESS) {
        // TEST LIB LOOPBACK
        // usart_putchar(DBG_USART,c);
        // print_dbg_char(c);

        serial_buffer[serial_write_pos] = c;
        serial_write_pos++;
        if (serial_write_pos == SERIAL_BUFFER_SIZE) serial_write_pos = 0;
    }
    e.type = kEventSerial;
    e.data = serial_write_pos;
    event_post(&e);
}
