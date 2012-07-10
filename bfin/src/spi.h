#ifndef _ALEPH_BFIN_SPI_H_
#define _ALEPH_BFIN_SPI_H_

#include "types.h"

//-------- global variables
// receive message data
extern msg_t rxMsg;
// transmit message data
extern msg_t txMsg;
// SPI byte counter
extern u8 spiByte;
// final byte index for current transaction
extern u8 spiLastByte;

//------- function prototypes
// handle new data in the spi RX ringbuffer
void handle_spi_rx(void);

#endif // header guard
