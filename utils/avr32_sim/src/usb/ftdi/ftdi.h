/* ftdi.h
   
   ftdi driver for monome aleph
 */

#ifndef _ALEPH_FTD_H_
#define _ALEPH_FTDI_H_

#include "types.h"
//#include "uhc.h"

#define FTDI_RX_BUF_SIZE 64
//#define FTDI_TX_BUF_SIZE 64

// ftdi sends 2 status bytes at top of each packet.
// seems to usually be 0x31 0x60
#define FTDI_STATUS_BYTES 2

// pointer to rx data
///extern volatile u8 ftdiRxBuf[];
// num bytes read
//extern volatile u32 ftdiRxBytes;

// read from FTDI device on usb.
// returns bytes read.
// data is in the externally-visible rx buffer.
//extern u32 ftdi_read(void);
// non-blocking
extern void ftdi_read(void);

// write to FTDI device
extern void ftdi_write(u8* data, u32 bytes);

// FTDI device was plugged or unplugged
//extern void ftdi_change(uhc_device_t* dev, u8 plug);
// main-loop setup routine for new device connection
extern void ftdi_setup(void);

//-- getters

// rx buffer (no status bytes)
extern u8* ftdi_rx_buf(void);
// number of bytes from last rx trasnfer
extern volatile u8 ftdi_rx_bytes(void);
// busy flags
extern volatile u8 ftdi_rx_busy(void);
extern volatile u8 ftdi_tx_busy(void);
// device connected flag
extern u8 ftdi_connected(void);

#endif // h guard
