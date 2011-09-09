#ifndef _ALEPH_BFIN_SPI_H_
#define _ALEPH_BFIN_SPI_H_

#include "types.h"

//-------- global variables
// ringbuffer for spi rx data
extern u16 spiRxRing[];
// counter 
extern u16 spiRxRingIdx;

//------- function prototypes
// handle new data in the spi RX ringbuffer
void handle_spi_rx(void);

#endif // header guard
