#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

// global count of uptime, and overflow flag.
extern volatile u64 tcTicks;
extern volatile u8 tcOverflow;

extern void register_interrupts(void);

#endif 

