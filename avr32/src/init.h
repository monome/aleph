/* init.h
   aleph-avr32

   low-level peripheral  initialization.
*/

#ifndef _INIT_H_
#define _INIT_H_

// initialize clocks
// extern void init_clocks(void);

// initialize blackfin SPI and GPIPO
extern void init_bfin_resources(void);
// initialize USART
extern void init_ftdi_usart(void);
// initialize other GPIO
extern void init_gpio(void);
// initialize PDCA for sdcard
extern void init_local_pdca(void);
// initialize spi1 (oled / adc / sdcard)
extern void init_spi1(void);
// initialize application timer
extern void init_tc(void);
// intialize two-wire interface
extern void init_twi(void);
// initialize SPI and GPIO for sdcard
extern void init_sd_mmc_resources(void);
// initialize USB host stack
void init_usb_host(void);

#endif
