#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#ifdef __cplusplus
extern "C" {
#endif

// global count of uptime, and overflow flag.
volatile u64 tcTicks;
volatile u8 tcOverflow;

extern void register_interrupts(void);

#ifdef __cplusplus
}
#endif

#endif 

