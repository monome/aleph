#ifndef _ALEPH_FTD_H_
#define _ALEPH_FTDI_H_


#include "types.h"
#include "uhc.h"

#define FTDI_IN_BUF_SIZE 64
#define FTDI_OUT_BUF_SIZE 64

// read from FTDI device on usb
extern void ftdi_read(void);
// write to FTDI device
extern void ftdi_write(u32 data);
// FTDI device was plugged or unplugged
extern void ftdi_change(uhc_device_t* dev, u8 plug);
// main-loop setup routine for new device connection
extern void ftdi_setup(void);

#endif // h guard
