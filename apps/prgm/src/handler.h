//handler.h
//aleph-prgm-avr32

//rename to global.h

#ifndef _ALEPH_APP_PRGM_HANDLER_H_
#define _ALEPH_APP_PRGM_HANDLER_H_

extern void assign_prgm_event_handlers(void);

extern void adc_init(void);

extern void step_advance(s32 val);
extern void step_advance_t(s32 val);

//sync trig variables
#define ON 1
#define OFF 0
u8 state;

#endif 
