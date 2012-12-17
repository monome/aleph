#ifndef _ALEPH_BFIN_INIT_H_
#define _ALEPH_BFIN_INIT_H_

//--------- function prototypes
void init_clocks(void);
void init_dma(void);
void init_interrupts(void);
void init_sport1(void);
void init_timers(void);
void enable_sport1_dma(void);

#endif // header guard
