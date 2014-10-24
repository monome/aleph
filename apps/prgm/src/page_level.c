//page_level.c
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

//prgm
#include "handler.h"
#include "pages.h"
#include "page_level.h"
#include "render.h"
#include "ctl.h"
#include "util.h"
#include "scale.h"

//tracker variables
fstep *step[F_STEPS_MAX];

u8 counter;
u8 length = F_STEPS_MAX;

//tracker functions (static)
static Steppointer_f init(void);

static void init_fstep(fstep *step);

Steppointer_f init(void) {
    return(Steppointer_f)alloc_mem(sizeof(fstep));
}

void init_fstep(fstep *s) {
    fstep *step = (fstep*)s;
    
    step->osc0_f = INIT_F;
    step->osc1_f = INIT_F;
    step->osc2_f = INIT_F;
    step->osc3_f = INIT_F;
    step->osc0_t = INIT_T;
    step->osc1_t = INIT_T;
    step->osc2_t = INIT_T;
    step->osc3_t = INIT_T;

    step = step->next;
}

void tracker_init(void) {
    u8 s;
        
    for (s=0; s<F_STEPS_MAX; s++) {
        step[s] = init();
        init_fstep(step[s]);
    }
    counter = 0;
}

//handler function declarations
static inline void handle_sw(u8 id, u8 b);
static void handle_switch_0(s32 data);
static void handle_switch_1(s32 data);
static void handle_switch_2(s32 data);
static void handle_switch_3(s32 data);
static void handle_switch_4(s32 data);

static inline u8 check_touch(etype et);
static void handle_encoder_0(s32 val);
static void handle_encoder_1(s32 val);
static void handle_encoder_2(s32 val);
static void handle_encoder_3(s32 val);

//handler variables
static etype touched = kNumEventTypes; //total number as defined in event_types.h
static u8 touchedThis = 0;
static u8 state_sw;

//switch3 trig
#define ON 1
#define OFF 0
static u8 switch3;

//handler functions (static)
u8 check_touch(etype et) {
    if(touched != et) {
        touchedThis = 1;
        touched = et;
    }
    return touchedThis;
}

void handle_sw(u8 id, u8 b) {
    if(b) {
        state_sw = id;
    }
    else if (!b) {
        state_sw = 0;
    }
}

void handle_switch_0(s32 data) {
    handle_sw(1, data > 0);
}

void handle_switch_1(s32 data) {
    handle_sw(2, data > 0);
}

void handle_switch_2(s32 data) {
    //
}

void handle_switch_3(s32 data) {
    if(data == 0)
        switch3 = OFF;
    
    else if(switch3 == OFF) {
        switch3 = ON;
        step_advance();
    }
    
    else if(switch3 == ON)
        ;;
}

void handle_switch_4(s32 data) {
    if(data == 0) { return; }
    
    else {
        set_page(ePageEnv);
        render_env();
    }
}

void handle_encoder_0(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                Free0 += val * 4096;
                if (Free0 < 0) Free0 = 0;
                step[counter]->osc0_f = Free0;
                ctl_param_change(eParamFree0, Free0);
                print_fix16(renderFree0, fix16_mul(Free0, Transposed0));
                render_free();
                }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                Free0 += val * 4194304;
                if (Free0 < 0) Free0 = 0;
                step[counter]->osc0_f = Free0;
                ctl_param_change(eParamFree0, Free0);
                print_fix16(renderFree0, fix16_mul(Free0, Transposed0));
                render_free();
            }
            break;

        case 2:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                Transposed0tmp += val;
                //check for lower limit
                if (Transposed0tmp < 0) Transposed0tmp = 0;
                //check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(Transposed0tmp) == 0) { Transposed0 = 2 * 0x00010000; Transposed0tmp -= val; }
                Transposed0 = transpose_lookup(Transposed0tmp) * 0x00010000;
                step[counter]->osc0_t = Transposed0;
                ctl_param_change(eParamTransposed0, Transposed0);
                print_fix16(renderFree0, fix16_mul(Free0, Transposed0));
                print_fix16(renderTransposed0, Transposed0);
                render_free();
                render_transposed();
            }
            break;
            
        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Free1 += val * 4096;
                if (Free1 < 0) Free1 = 0;
                step[counter]->osc1_f = Free1;
                ctl_param_change(eParamFree1, Free1);
                print_fix16(renderFree1, fix16_mul(Free1, Transposed1));
                render_free();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Free1 += val * 4194304;
                if (Free1 < 0) Free1 = 0;
                step[counter]->osc1_f = Free1;
                ctl_param_change(eParamFree1, Free1);
                print_fix16(renderFree1, fix16_mul(Free1, Transposed1));
                render_free();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Transposed1tmp += val;
                //check for lower limit
                if (Transposed1tmp < 0) Transposed1tmp = 0;
                //check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(Transposed1tmp) == 0) { Transposed1 = 2 * 0x00010000; Transposed1tmp -= val; }
                Transposed0 = transpose_lookup(Transposed1tmp) * 0x00010000;
                step[counter]->osc1_t = Transposed1;
                ctl_param_change(eParamTransposed1, Transposed1);
                print_fix16(renderFree1, fix16_mul(Free1, Transposed1));
                print_fix16(renderTransposed1, Transposed1);
                render_free();
                render_transposed();
            }
            break;
            
        default:
            break;
    }
}

void handle_encoder_2(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Free2 += val * 4096;
                if (Free2 < 0) Free2 = 0;
                step[counter]->osc2_f = Free2;
                ctl_param_change(eParamFree2, Free2);
                print_fix16(renderFree2, fix16_mul(Free2, Transposed2));
                render_free();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Free2 += val * 4194304;
                if (Free2 < 0) Free2 = 0;
                step[counter]->osc2_f = Free2;
                ctl_param_change(eParamFree2, Free2);
                print_fix16(renderFree2, fix16_mul(Free2, Transposed2));
                render_free();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Transposed2tmp += val;
                //check for lower limit
                if (Transposed2tmp < 0) Transposed2tmp = 0;
                //check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(Transposed2tmp) == 0) { Transposed2 = 2 * 0x00010000; Transposed2tmp -= val; }
                Transposed2 = transpose_lookup(Transposed2tmp) * 0x00010000;
                step[counter]->osc2_t = Transposed2;
                ctl_param_change(eParamTransposed2, Transposed2);
                print_fix16(renderFree2, fix16_mul(Free2, Transposed2));
                print_fix16(renderTransposed2, Transposed2);
                render_free();
                render_transposed();
            }
            break;
            
        default:
            break;
    }
}

void handle_encoder_3(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Free3 += val * 4096;
                if (Free3 < 0) Free3 = 0;
                step[counter]->osc3_f = Free3;
                ctl_param_change(eParamFree3, Free3);
                print_fix16(renderFree3, fix16_mul(Free3, Transposed3));
                render_free();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Free3 += val * 4194304;
                if (Free3 < 0) Free3 = 0;
                step[counter]->osc3_f = Free3;
                ctl_param_change(eParamFree3, Free3);
                print_fix16(renderFree3, fix16_mul(Free3, Transposed3));
                render_free();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Transposed3tmp += val;
                //check for lower limit
                if (Transposed3tmp < 0) Transposed3tmp = 0;
                //check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(Transposed3tmp) == 0) { Transposed3 = 2 * 0x00010000; Transposed3tmp -= val; }
                Transposed3 = transpose_lookup(Transposed3tmp) * 0x00010000;
                step[counter]->osc3_t = Transposed3;
                ctl_param_change(eParamTransposed3, Transposed3);
                print_fix16(renderFree3, fix16_mul(Free3, Transposed3));
                print_fix16(renderTransposed3, Transposed3);
                render_free();
                render_transposed();
            }
            break;
            
        default:
            break;
    }
}

void step_advance() {
    s32 tmp0;
    s32 tmp1;
    s32 tmp2;
    s32 tmp3;

    s32 tmp4;
    s32 tmp5;
    s32 tmp6;
    s32 tmp7;
        
    if (counter < length - 1) {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        tmp0 = Free0;
        tmp1 = Free1;
        tmp2 = Free2;
        tmp3 = Free3;
        
        tmp4 = Transposed0;
        tmp5 = Transposed1;
        tmp6 = Transposed2;
        tmp7 = Transposed3;
        
        step[counter]->osc0_f = tmp0;
        step[counter]->osc1_f = tmp1;
        step[counter]->osc2_f = tmp2;
        step[counter]->osc3_f = tmp3;
        
        step[counter]->osc0_t = tmp4;
        step[counter]->osc1_t = tmp5;
        step[counter]->osc2_t = tmp6;
        step[counter]->osc3_t = tmp7;
        
        counter++;
        
        Free0 = step[counter]->osc0_f;
        Free1 = step[counter]->osc1_f;
        Free2 = step[counter]->osc2_f;
        Free3 = step[counter]->osc3_f;
        
        Transposed0 = step[counter]->osc0_t;
        Transposed1 = step[counter]->osc1_t;
        Transposed2 = step[counter]->osc2_t;
        Transposed3 = step[counter]->osc3_t;
        
        ctl_param_change(eParamTransposed0, Transposed0);
        ctl_param_change(eParamTransposed1, Transposed1);
        ctl_param_change(eParamTransposed2, Transposed2);
        ctl_param_change(eParamTransposed3, Transposed3);
        
        ctl_param_change(eParamFree0, Free0);
        ctl_param_change(eParamFree1, Free1);
        ctl_param_change(eParamFree2, Free2);
        ctl_param_change(eParamFree3, Free3);
        
        print_fix16(renderFree0, fix16_mul(Free0, Transposed0));
        print_fix16(renderFree1, fix16_mul(Free1, Transposed1));
        print_fix16(renderFree2, fix16_mul(Free2, Transposed2));
        print_fix16(renderFree3, fix16_mul(Free3, Transposed3));
        
        print_fix16(renderTransposed0, Transposed0);
        print_fix16(renderTransposed1, Transposed1);
        print_fix16(renderTransposed2, Transposed2);
        print_fix16(renderTransposed3, Transposed3);
        
        print_fix16(renderCounter, (counter + 1) * 0x00010000);
        
        if (pageIdx != ePageEnv) { render_free(); render_transposed(); render_counters(); }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
    
    else if (counter < length) {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        tmp0 = Free0;
        tmp1 = Free1;
        tmp2 = Free2;
        tmp3 = Free3;
        
        tmp4 = Transposed0;
        tmp5 = Transposed1;
        tmp6 = Transposed2;
        tmp7 = Transposed3;
        
        step[counter]->osc0_f = tmp0;
        step[counter]->osc1_f = tmp1;
        step[counter]->osc2_f = tmp2;
        step[counter]->osc3_f = tmp3;
        
        step[counter]->osc0_t = tmp4;
        step[counter]->osc1_t = tmp5;
        step[counter]->osc2_t = tmp6;
        step[counter]->osc3_t = tmp7;
        
        counter = 0;
        
        Free0 = step[counter]->osc0_f;
        Free1 = step[counter]->osc1_f;
        Free2 = step[counter]->osc2_f;
        Free3 = step[counter]->osc3_f;
        
        Transposed0 = step[counter]->osc0_t;
        Transposed1 = step[counter]->osc1_t;
        Transposed2 = step[counter]->osc2_t;
        Transposed3 = step[counter]->osc3_t;
        
        ctl_param_change(eParamTransposed0, Transposed0);
        ctl_param_change(eParamTransposed1, Transposed1);
        ctl_param_change(eParamTransposed2, Transposed2);
        ctl_param_change(eParamTransposed3, Transposed3);
        
        ctl_param_change(eParamFree0, Free0);
        ctl_param_change(eParamFree1, Free1);
        ctl_param_change(eParamFree2, Free2);
        ctl_param_change(eParamFree3, Free3);
        
        print_fix16(renderFree0, fix16_mul(Free0, Transposed0));
        print_fix16(renderFree1, fix16_mul(Free1, Transposed1));
        print_fix16(renderFree2, fix16_mul(Free2, Transposed2));
        print_fix16(renderFree3, fix16_mul(Free3, Transposed3));
        
        print_fix16(renderTransposed0, Transposed0);
        print_fix16(renderTransposed1, Transposed1);
        print_fix16(renderTransposed2, Transposed2);
        print_fix16(renderTransposed3, Transposed3);
        
        print_fix16(renderCounter, (counter + 1) * 0x00010000);
        
        if (pageIdx != ePageEnv) { render_free(); render_transposed(); render_counters(); }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

void select_level(void) {
//assign tracker handlers
    app_event_handlers[ kEventEncoder0 ] = &handle_encoder_3 ;
    app_event_handlers[ kEventEncoder1 ] = &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder2 ] = &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder3 ] = &handle_encoder_0 ;
    app_event_handlers[ kEventSwitch0 ] = &handle_switch_0 ;
    app_event_handlers[ kEventSwitch1 ] = &handle_switch_1 ;
    app_event_handlers[ kEventSwitch2 ] = &handle_switch_2 ;
    app_event_handlers[ kEventSwitch3 ] = &handle_switch_3 ;
    app_event_handlers[ kEventSwitch4 ] = &handle_switch_4 ;
}
