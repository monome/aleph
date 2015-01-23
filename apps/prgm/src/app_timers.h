//app_timers.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_TIMERS_H_
#define _ALEPH_APP_PRGM_TIMERS_H_

#include "timers.h"
#include "types.h"

//adc variables and trig states
#define ARM 2
#define ON 1
#define OFF 0
u16 adc[4];
u8 state;
u16 previous_value;

//foot control states
u8 foot1_touched;
u8 foot2_touched;

//bfin reported states from custom_callback()
u8 bfinheadstate;
u8 bfinheadpos;


// create application timers
extern void init_app_timers(void);

// start adc polling timer
extern void timers_set_adc(u32 period);

// stop adc polling timer
extern void timers_unset_adc(void);

// change period of adc polling timer
extern void timers_set_adc_period(u32 period);

#endif
