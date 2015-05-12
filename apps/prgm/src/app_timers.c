//app_timers.c
//aleph-prgm-avr32

/*
this is where callbacks are declared for all application-specific software timers.
 these callbacks are performed from the TC interrupt service routine.
 therefore, they should be kept small.
 
 avr32_lib defines a custom application event type, 
 which should be used when timer-based processing needs to be deferred to the main loop.
*/

#include "app_timers.h"

//static variables
static event_t e;
static u16 adc[4];
static u8 state;
static u16 previous_value;

//TIMERS
// poll adc
static softTimer_t adcPollTimer = { .next = NULL, .prev = NULL };

// refresh screen
static softTimer_t screenTimer = { .next = NULL, .prev = NULL };

// poll encoders
static softTimer_t encTimer = { .next = NULL, .prev = NULL };

//poll custom
static softTimer_t customPollTimer = { .next = NULL, .prev = NULL };

// poll monome device
//static softTimer_t monomePollTimer = { .next = NULL, .prev = NULL };
// refresh monome device
//static softTimer_t monomeRefreshTimer  = { .next = NULL, .prev = NULL };
// poll midi device
//static softTimer_t midiPollTimer = { .next = NULL, .prev = NULL };

//adc polling callback
static void adc_poll_timer_callback(void *obj) {
    static const int threshold = 50;
    
    adc_convert(&adc);
    u16 i = adc[0];
    s16 diff = i - previous_value;
    
    if (state == 0)
    {
        //  wait for trig
        if (diff > threshold)
        {
            //  trig found
            e.type = kEventAdc0;
            e.data = 1;
            event_post(&e);            
            state = 1;
        }
        
    } else
    {
        //  wait for end of trig        
        if (diff < -threshold)
        {
            //  end of trig found
            state = 0;
        }
    }
    previous_value = i;
}


//screen refresh callback
static void screen_timer_callback(void* obj) {
    render_update();
}


//encoder polling callback
static void enc_timer_callback(void* obj) {
    static s16 val, valAbs;
    u8 i;
        
    for(i=0; i<NUM_ENC; i++) {
        val = enc[i].val;
        valAbs = (val & 0x8000 ? (val ^ 0xffff) + 1 : val);
        if(valAbs > enc[i].thresh) {
            e.type = enc[i].event;
            e.data = val;
            enc[i].val = 0;
            event_post(&e);
        }
    }
}


//custom callback
static void custom_timer_callback(void *obj) {
    ParamValueSwap s;
    
    while (idx8 < bfinSampleSize)
    {
        ctl_param_change(DUMMY, eParamOffset, sample[smpl]->offset + idx32);
        
        s.asByte[0] = bfinSampleData[idx8];
        s.asByte[1] = bfinSampleData[idx8 + 1];
        s.asByte[2] = bfinSampleData[idx8 + 2];
        s.asByte[3] = bfinSampleData[idx8 + 3];
        
        ctl_param_change(DUMMY, eParamSample, s.asInt);
                
        idx8 += 4;
        idx32++;
    }
}


//external functions
void init_sample_timer(void) {
    timer_add(&customPollTimer, 1, &custom_timer_callback, NULL);
}

void deinit_sample_timer(void) {
    customPollTimer.ticks = 50000;
//    timer_remove(&customPollTimer);
}

void init_app_timers(void) {
    timer_add(&screenTimer, 50, &screen_timer_callback, NULL);
    timer_add(&encTimer, 50, &enc_timer_callback, NULL);
}

//init adc, set adc timers
void adc_init(void) {
    timers_set_adc(1);
}

//start adc polling
void timers_set_adc(u32 period) {
    timer_add(&adcPollTimer, period, &adc_poll_timer_callback, NULL);
}

//set adc polling period
void timers_set_adc_period(u32 period) {
    adcPollTimer.ticks = period;
}
