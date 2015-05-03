//app_timers.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_TIMERS_H_
#define _ALEPH_APP_PRGM_TIMERS_H_

#include "timers.h"
#include "types.h"

//asf
#include "print_funcs.h"

//aleph-avr32
#include "control.h"
#include "encoders.h"
#include "events.h"
#include "midi.h"
#include "flash.h"

//prgm and customized library
#include "adc.h"
#include "app_timers.h"
#include "render.h"
#include "ctl.h"
#include "files.h"
#include "tracker.h"
#include "generator.h"

//mode led ctrl
#include "gpio.h"
#include "aleph_board.h"

//trig
#include "bfin.h"

// create application timers
extern void init_sample_timer(void);
extern void deinit_sample_timer(void);
extern void init_app_timers(void);

// start adc polling timer
extern void adc_init(void);
extern void timers_set_adc(u32 period);

// stop adc polling timer
extern void timers_unset_adc(void);

// change period of adc polling timer
extern void timers_set_adc_period(u32 period);

#endif
