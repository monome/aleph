//tracker.c
//aleph-prgm-avr32

// asf
#include "gpio.h"
#include "print_funcs.h"

// common
#include "fix.h"

// avr32
#include "region.h"
#include "app.h"
#include "control.h"
#include "memory.h"

// bees
#include "handler.h"
#include "pages.h"
#include "render.h"
#include "ctl.h"
#include "util.h"
#include "tracker.h"


//tracker variables
tstep *step[N_STEPS];

u8 counter = 0;


//tracker functions (static)
static Steppointer init(void);
static void init_tstep(tstep *step);
static void set_step(u8 s); 

Steppointer init(void) {
    return(Steppointer)alloc_mem(sizeof(tstep));
}

void init_tstep(tstep *s) {
    tstep *step = (tstep*)s;
    
    step->osc0_f = 220 * 0x00010000;
    step->osc1_f = 220 * 0x00010000;
    step->osc2_f = 220 * 0x00010000;
    step->osc3_f = 220 * 0x00010000;
    step = step->next;
}

void set_step(u8 s) {
    step[s]->osc0_f = Freq0;
    step[s]->osc1_f = Freq1;
    step[s]->osc2_f = Freq2;
    step[s]->osc3_f = Freq3;
}


//tracker functions (external)
void tracker_init(void) {
    u8 s;
        
    for (s=0; s<N_STEPS; s++) {
        step[s] = init();
        init_tstep(step[s]);
    }
}


//handler functions (static)
static void handle_encoder_0(s32 val);
static void handle_encoder_1(s32 val);
static void handle_encoder_2(s32 val);
static void handle_encoder_3(s32 val);
static void handle_key_0(s32 val);
static void handle_key_1(s32 val);
//static void handle_key_3(s32 val);
static void handle_key_4(s32 val);
static void handle_switch_3_6(s32 val);
static inline u8 check_touch(etype et);

//handler variables
static etype touched = kNumEventTypes;
static u8 touchedThis = 0;

u8 check_touch(etype et) {
    if(touched != et) {
        touchedThis = 1;
        touched = et;
    }
    return touchedThis;
}

void handle_encoder_0(s32 val) {
    check_touch(kEventEncoder3);
    if (touchedThis) {
        Freq0 += val * 0x00010000;
        set_step(counter);
        ctl_param_change(eParamFreq0, Freq0);
        print_fix16(renderFreq0, Freq0);
        render_freq();
    }
}

void handle_encoder_1(s32 val) {
    check_touch(kEventEncoder2);
    if (touchedThis) {
        Freq1 += val * 0x00010000;
        set_step(counter);
        ctl_param_change(eParamFreq1, Freq1);
        print_fix16(renderFreq1, Freq1);
        render_freq();
    }
}

void handle_encoder_2(s32 val) {
    check_touch(kEventEncoder1);
    if (touchedThis) {
        Freq2 += val * 0x00010000;
        set_step(counter);
        ctl_param_change(eParamFreq2, Freq2);
        print_fix16(renderFreq2, Freq2);
        render_freq();
    }
}

void handle_encoder_3(s32 val) {
    check_touch(kEventEncoder0);
    if (touchedThis) {
        Freq3 += val * 0x00010000;
        set_step(counter);
        ctl_param_change(eParamFreq3, Freq3);
        print_fix16(renderFreq3, Freq3);
        render_freq();
    }
}

void handle_key_0(s32 val) {
    if(val == 0) { return; }
    
    else {
        ctl_param_change(eParamSyncTrig, 1);
        ctl_param_change(eParamSyncTrig, 0);
    }
}

void handle_key_1(s32 val) {
    //nothing
}

//MAKE THIS GLOBAL!
void handle_switch_3_6(s32 val) {
    s32 tmp0;
    s32 tmp1;
    s32 tmp2;
    s32 tmp3;
    
    if(val == 0) { return; }
    
    else if (counter < N_STEPS - 1) {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        tmp0 = Freq0;
        tmp1 = Freq1;
        tmp2 = Freq2;
        tmp3 = Freq3;

        step[counter]->osc0_f = tmp0;
        step[counter]->osc1_f = tmp1;
        step[counter]->osc2_f = tmp2;
        step[counter]->osc3_f = tmp3;

        counter++;
        
        Freq0 = step[counter]->osc0_f;
        Freq1 = step[counter]->osc1_f;
        Freq2 = step[counter]->osc2_f;
        Freq3 = step[counter]->osc3_f;
                
        ctl_param_change(eParamFreq0, Freq0);
        ctl_param_change(eParamFreq1, Freq1);
        ctl_param_change(eParamFreq2, Freq2);
        ctl_param_change(eParamFreq3, Freq3);
        
        print_fix16(renderFreq0, Freq0);
        print_fix16(renderFreq1, Freq1);
        print_fix16(renderFreq2, Freq2);
        print_fix16(renderFreq3, Freq3);        
        
        render_freq();
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
    
    else if (counter < N_STEPS) {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        tmp0 = Freq0;
        tmp1 = Freq1;
        tmp2 = Freq2;
        tmp3 = Freq3;
        
        step[counter]->osc0_f = tmp0;
        step[counter]->osc1_f = tmp1;
        step[counter]->osc2_f = tmp2;
        step[counter]->osc3_f = tmp3;
                
        counter = 0;
        
        Freq0 = step[counter]->osc0_f;
        Freq1 = step[counter]->osc1_f;
        Freq2 = step[counter]->osc2_f;
        Freq3 = step[counter]->osc3_f;

        ctl_param_change(eParamFreq0, Freq0);
        ctl_param_change(eParamFreq1, Freq1);
        ctl_param_change(eParamFreq2, Freq2);
        ctl_param_change(eParamFreq3, Freq3);
        
        print_fix16(renderFreq0, Freq0);
        print_fix16(renderFreq1, Freq1);
        print_fix16(renderFreq2, Freq2);
        print_fix16(renderFreq3, Freq3);

        render_freq();
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

void handle_key_4(s32 val) {
    if(val == 0) { return; }
    
    else {
        set_page(ePagePrgm);
        render_prgm();
    }
}


//handler functions (external)
void select_tracker(void) {
//assign tracker handlers
    app_event_handlers[ kEventEncoder0 ] = &handle_encoder_3 ;
    app_event_handlers[ kEventEncoder1 ] = &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder2 ] = &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder3 ] = &handle_encoder_0 ;
    app_event_handlers[ kEventSwitch0 ] = &handle_key_0 ;
    app_event_handlers[ kEventSwitch1 ] = &handle_key_1 ;
    //    app_event_handlers[ kEventSwitch2 ]	= &handle_key_2 ;
    app_event_handlers[ kEventSwitch3 ] = &handle_switch_3_6 ;
    app_event_handlers[ kEventSwitch4 ] = &handle_key_4 ;
    app_event_handlers[ kEventSwitch6 ] = &handle_switch_3_6 ;
}
