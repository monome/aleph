/* init.h
   aleph-avr32
   
   low-level peripheral  initialization.
*/

#ifndef _INIT_H_
#define _INIT_H_

// initialize clocks
extern void init_clocks(void);
// initialize blackfin SPI and GPIPO
extern void init_bfin_resources(void);
// initialize debug USART
extern void init_ftdi_usart (void);
// initialize OLED USART in SPI mode
extern void init_oled_spi (void);
// initialize application timer
extern void init_tc (volatile avr32_tc_t *tc);
// initialize PDCA for sdcard
extern void init_local_pdca(void);
// initialize SPI and GPIO for sdcard
extern void init_sd_mmc_resources(void);
// initialize other GPIO
extern void init_gpio(void);

#endif
