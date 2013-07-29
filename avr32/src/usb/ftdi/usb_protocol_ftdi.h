#ifndef _USB_FTDI_PROTOCOL_H_
#define _USB_FTDI_PROTOCOL_H_

/* FTDI data taken from descriptors */
#define FTDI_ADDR        1
#define FTDI_VID        0x0403  // FTDI VID
#define FTDI_PID        0x6001  // FTDI PID
#define FTDI_CONFIGURATION 1_
#define FTDI_INTERFACE    0 // Only use interface 0
#define FTDI_NUM_EP      3


// generic USB parameters, probly dont need
//#define BULK_MAXPKTSIZE  64
//#define EP_BULK         0x02
//#define USB_NAK_NOWAIT       1

//#define CONTROL_PIPE      0 // names we give to the 3 pipes
//#define DATAIN_PIPE       1
//#define DATAOUT_PIPE      2

#define  FTDI_CLASS                  0xFF
#define  FTDI_SUBCLASS               0xFF
#define  FTDI_PROTOCOL               0xFF

#endif // h guard
