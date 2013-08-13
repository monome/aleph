#ifndef _ALEPH_BFIN_SPI_H_
#define _ALEPH_BFIN_SPI_H_

#include "param_common.h"
#include "types.h"

/* //----- defines */
/* #define SPI_FIFO_SIZE 32 */

/* //---- types */

/* typedef struct _spiEvent { */
/*   u8 idx; */
/*   pval val; */
/* } spiEvent_t; */

//-------- global variables
// parameter change queue


// receive message data
//#define SPI_RX_BUF_NUM 

// extern u8 spiBuf;
//extern u8 spiRxIdx = 0;
//extern u8 spiProcIdx = 0;

/*
extern msg_t rxMsg;
// transmit message data
extern msg_t txMsg;
// SPI byte counter
extern u8 spiByte;
// final byte index for current transaction
extern u8 spiLastByte;
*/




//------- function prototypes
// handle new data in the spi RX ringbuffer
// return tx data
u8 spi_process(u8 rx);

#endif // header guard
