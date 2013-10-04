#ifndef _ALEPH_BFIN_INIT_H_
#define _ALEPH_BFIN_INIT_H_


//// OUTPUTS:
// led3, led4 : pf14,15 
// codec reset : pf1
#define PF_DIR           0xc002 
#define CODEC_RESET_MASK 0xfffd 
#define CODEC_SS_MASK    0xffdf

//--------- function prototypes
void init_clocks(void);
void init_dma(void);
void init_flags(void);
void init_interrupts(void);
void init_sport1(void);
void init_timers(void);
void enable_sport1_dma(void);

#endif // header guard
