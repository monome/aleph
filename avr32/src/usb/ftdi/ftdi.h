#ifndef _ALEPH_FTD_H_
#define _ALEPH_FTDI_H_


#include "types.h"
#include "uhc.h"

#define FTDI_IN_BUF_SIZE 64
#define FTDI_OUT_BUF_SIZE 64

// pointer to rx data
extern u8 ftdiRxBuf[];

// read from FTDI device on usb.
// returns bytes read.
// data is in the externally-visible rx buffer.
extern u32 ftdi_read(void);

// write to FTDI device
extern void ftdi_write(u8* data, u32 bytes);

// FTDI device was plugged or unplugged
extern void ftdi_change(uhc_device_t* dev, u8 plug);
// main-loop setup routine for new device connection
extern void ftdi_setup(void);

#endif // h guard
