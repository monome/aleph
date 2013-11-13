/* init.h
   aleph-avr32
   
   low-level peripheral  initialization.
*/

#ifndef _INIT_H_
#define _INIT_H_


// initialize clocks
//extern void init_clocks(void);

// initialize blackfin SPI and GPIPO
extern void init_bfin_resources(void);
// initialize USART
extern void init_ftdi_usart (void);
// initialize other GPIO
extern void init_gpio(void);
// initialize PDCA for sdcard
extern void init_local_pdca(void);
// initialize spi1 (oled / adc / sdcard)
extern void init_spi1 (void);
// initialize application timer
extern void init_tc (volatile avr32_tc_t *tc);

// initialize SPI and GPIO for sdcard
extern void init_sd_mmc_resources(void);
// initialize USB host stack
void init_usb_host (void);


// intialize TWI
extern void init_i2c(void);
extern void init_i2c_slave(void);
extern void init_i2c_master(void);


#endif
