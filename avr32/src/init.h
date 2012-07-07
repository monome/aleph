/* init.h
   aleph-avr32
   
   low-level peripheral  initialization.
*/

#ifndef _INIT_H_
#define _INIT_H_

extern void init_clocks(void);
extern void init_bfin_resources(void);
extern void init_dbg_usart (void);
extern void init_local_pdca(void);
extern void init_sd_mmc_resources(void);

#endif
