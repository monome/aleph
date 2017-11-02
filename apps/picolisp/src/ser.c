/* #include <stdio.h> */
/* #include <avr/io.h> */
#include "ser.h"
#include "serial.h"
#include "usart.h"
#include "aleph_board.h"
#include "pico.h"
#include "print_funcs.h"


static u16 serial_read_pos = 0;
int lispInitialised = 0;
void recv_char (char c) {
  usart_putchar(DEV_USART, c);
  if (c == '\b') {
    serialInputLineBufferPos--;
  } else {
    serialInputLineBuffer[serialInputLineBufferPos] = c;
    serialInputLineBufferPos++;
  }
  if (c == '\n' || c == '\r') {
    /* stdout = fdevopen(test_write); */


    serialInputLineBuffer[serialInputLineBufferPos] = '\n';
    serialInputLineBufferPos++;
    serialInputLineBuffer[serialInputLineBufferPos] = '\0';
    serialInputLineBufferPos++;

    usart_putchar(DEV_USART, '\r');
    usart_putchar(DEV_USART, '\n');

    readLispString(serialInputLineBuffer);
    serialInputLineBufferPos = 0;
  }
}

void handle_Serial (s32 data) {
  u16 c = (u16)data;
  while(serial_read_pos != c) {
    recv_char(serial_buffer[serial_read_pos]);
    serial_read_pos++;
    if(serial_read_pos == SERIAL_BUFFER_SIZE) {
      serial_read_pos = 0;
    }
  }
  /* serial_process(data); */
}


