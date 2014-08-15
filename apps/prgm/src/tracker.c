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
#include "scale.h"

//tracker variables
fstep *step[F_STEPS_MAX];
tstep *step_t[T_STEPS_MAX];

u8 counter;
u8 counter_t;
u8 length = F_STEPS_MAX;
u8 length_t = T_STEPS_MAX;

//tracker functions (static)
static Steppointer_f init(void);
static Steppointer_t init_t(void);

static void init_fstep(fstep *step);
static void init_tstep(tstep *step_t);

static void set_step(u8 s);
static void set_step_t(u8 s);

Steppointer_f init(void) {
    return(Steppointer_f)alloc_mem(sizeof(fstep));
}

Steppointer_t init_t(void) {
    return(Steppointer_t)alloc_mem(sizeof(tstep));
}

void init_fstep(fstep *s) {
    fstep *step = (fstep*)s;
    
    step->osc0_f = INIT_F;
    step->osc1_f = INIT_F;
    step->osc2_f = INIT_F;
    step->osc3_f = INIT_F;
    step = step->next;
}

void init_tstep(tstep *t) {
    tstep *step_t = (tstep*)t;
    
    step_t->osc0_t = INIT_T;
    step_t->osc1_t = INIT_T;
    step_t->osc2_t = INIT_T;
    step_t->osc3_t = INIT_T;
    step_t = step_t->next;
}

void set_step(u8 s) {
    step[s]->osc0_f = Freq0;
    step[s]->osc1_f = Freq1;
    step[s]->osc2_f = Freq2;
    step[s]->osc3_f = Freq3;
}

void set_step_t(u8 s) {
    step_t[s]->osc0_t = Transpose0;
    step_t[s]->osc1_t = Transpose1;
    step_t[s]->osc2_t = Transpose2;
    step_t[s]->osc3_t = Transpose3;
}

void tracker_init(void) {
    u8 s;
        
    for (s=0; s<F_STEPS_MAX; s++) {
        step[s] = init();
        init_fstep(step[s]);
    }

    for (s=0; s<T_STEPS_MAX; s++) {
        step_t[s] = init_t();
        init_tstep(step_t[s]);
    }
    counter = 0;
    counter_t = 0;
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
static etype touched = kNumEventTypes;
static u8 touchedThis = 0;
static u8 state_sw;

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
    step_advance(data);
}

void handle_switch_2(s32 data) {
    step_advance_t(data);
}

void handle_switch_3(s32 data) {
    handle_sw(4, data > 0);
}

void handle_switch_4(s32 data) {
    if(data == 0) { return; }
    
    else {
        set_page(ePagePrgm);
        render_prgm();
    }
}

void handle_encoder_0(s32 val) {
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    Freq0tmp += val;
                    if (Freq0tmp < 0) Freq0tmp = 0;
                    Freq0 = note_lookup(Freq0tmp) * 0x00010000;
                    set_step(counter);
                    print_dbg("\r\n eParamFreq0: ");
                    print_dbg_ulong(eParamFreq0);
                    ctl_param_change(eParamFreq0, Freq0);
                    print_dbg("\r\n eParamCVfreq0: ");
                    print_dbg_ulong(eParamCVfreq0);
                    ctl_param_change(eParamCVfreq0, Freq0);
                    print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
                    render_freq();
                }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
                if (touchedThis) {
                    Transpose0tmp += val;
                    if (Transpose0tmp < 0) Transpose0tmp = 0;
                    if (transpose_lookup(Transpose0tmp) == 0) { Transpose0 = 2 * 0x00010000; Transpose0tmp -= val; }
                    Transpose0 = transpose_lookup(Transpose0tmp) * 0x00010000;
                    set_step_t(counter_t);
                    print_dbg("\r\n eParamTranspose0: ");
                    print_dbg_ulong(eParamTranspose0);
                    ctl_param_change(eParamTranspose0, Transpose0);
                    print_dbg("\r\n eParamCVtranspose0: ");
                    print_dbg_ulong(eParamCVtranspose0);
                    ctl_param_change(eParamCVtranspose0, Transpose0);
                    print_fix16(renderTranspose0, Transpose0);
                    print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
                    render_freq();
                    render_transpose();
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
                Freq1tmp += val;
                if (Freq1tmp < 0) Freq1tmp = 0;
                Freq1 = note_lookup(Freq1tmp) * 0x00010000;
                set_step(counter);
                ctl_param_change(eParamFreq1, Freq1);
                ctl_param_change(eParamCVfreq1, Freq1);
                print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
                render_freq();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                Transpose1tmp += val;
                if (Transpose1tmp < 0) Transpose1tmp = 0;
                if (transpose_lookup(Transpose1tmp) == 0) { Transpose1 = 2 * 0x00010000; Transpose1tmp -= val; }
                Transpose1 = transpose_lookup(Transpose1tmp) * 0x00010000;
                set_step_t(counter_t);
                ctl_param_change(eParamTranspose1, Transpose1);
                ctl_param_change(eParamCVtranspose1, Transpose1);
                print_fix16(renderTranspose1, Transpose1);
                print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
                render_freq();
                render_transpose();
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
                Freq2tmp += val;
                if (Freq2tmp < 0) Freq2tmp = 0;
                Freq2 = note_lookup(Freq2tmp) * 0x00010000;
                set_step(counter);
                ctl_param_change(eParamFreq2, Freq2);
                ctl_param_change(eParamCVfreq2, Freq2);
                print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
                render_freq();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                Transpose2tmp += val;
                if (Transpose2tmp < 0) Transpose2tmp = 0;
                if (transpose_lookup(Transpose2tmp) == 0) { Transpose2 = 2 * 0x00010000; Transpose2tmp -= val; }
                Transpose2 = transpose_lookup(Transpose2tmp) * 0x00010000;
                set_step_t(counter_t);
                ctl_param_change(eParamTranspose2, Transpose2);
                ctl_param_change(eParamCVtranspose2, Transpose2);
                print_fix16(renderTranspose2, Transpose2);
                print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
                render_freq();
                render_transpose();
            }
            break;
            
        case 4:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                print_dbg("\r\n length: ");
                print_dbg_ulong(length);
                print_dbg("\r\n step: ");
                print_dbg_ulong(counter);
                length += val;
                if (length < 1) { length = 1; counter = 0; }
                if (length > F_STEPS_MAX) { length = F_STEPS_MAX; }
                while (counter > length - 1) { counter--; }

                Freq0 = step[counter]->osc0_f;
                Freq1 = step[counter]->osc1_f;
                Freq2 = step[counter]->osc2_f;
                Freq3 = step[counter]->osc3_f;
                
                ctl_param_change(eParamFreq0, Freq0);
                ctl_param_change(eParamFreq1, Freq1);
                ctl_param_change(eParamFreq2, Freq2);
                ctl_param_change(eParamFreq3, Freq3);
                
                ctl_param_change(eParamCVfreq0, Freq0);
                ctl_param_change(eParamCVfreq1, Freq1);
                ctl_param_change(eParamCVfreq2, Freq2);
                ctl_param_change(eParamCVfreq3, Freq3);
                
                print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
                print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
                print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
                print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));

                print_fix16(renderCounter, (counter + 1) * 0x00010000);

                render_freq();
                render_counters();
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
                Freq3tmp += val;
                if (Freq3tmp < 0) Freq3tmp = 0;
                Freq3 = note_lookup(Freq3tmp) * 0x00010000;
                set_step(counter);
                ctl_param_change(eParamFreq3, Freq3);
                ctl_param_change(eParamCVfreq3, Freq3);
                print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));
                render_freq();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                Transpose3tmp += val;
                if (Transpose3tmp < 0) Transpose3tmp = 0;
                if (transpose_lookup(Transpose3tmp) == 0) { Transpose3 = 2 * 0x00010000; Transpose3tmp -= val; }
                Transpose3 = transpose_lookup(Transpose3tmp) * 0x00010000;
                set_step_t(counter_t);
                ctl_param_change(eParamTranspose3, Transpose3);
                ctl_param_change(eParamCVtranspose3, Transpose3);
                print_fix16(renderTranspose3, Transpose3);
                print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));
                render_freq();
                render_transpose();
            }
            break;
            
        case 4:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                length_t += val;
                if (length_t < 1) { length_t = 1; counter_t = 0; }
                if (length_t > T_STEPS_MAX) { length_t = T_STEPS_MAX; }
                while (counter_t > length_t - 1) { counter_t--; }

                Transpose0 = step_t[counter_t]->osc0_t;
                Transpose1 = step_t[counter_t]->osc1_t;
                Transpose2 = step_t[counter_t]->osc2_t;
                Transpose3 = step_t[counter_t]->osc3_t;
                
                ctl_param_change(eParamTranspose0, Transpose0);
                ctl_param_change(eParamTranspose1, Transpose1);
                ctl_param_change(eParamTranspose2, Transpose2);
                ctl_param_change(eParamTranspose3, Transpose3);

                ctl_param_change(eParamFreq0, Freq0);
                ctl_param_change(eParamFreq1, Freq1);
                ctl_param_change(eParamFreq2, Freq2);
                ctl_param_change(eParamFreq3, Freq3);
                
                ctl_param_change(eParamCVtranspose0, Transpose0);
                ctl_param_change(eParamCVtranspose1, Transpose1);
                ctl_param_change(eParamCVtranspose2, Transpose2);
                ctl_param_change(eParamCVtranspose3, Transpose3);
                
                print_fix16(renderTranspose0, Transpose0);
                print_fix16(renderTranspose1, Transpose1);
                print_fix16(renderTranspose2, Transpose2);
                print_fix16(renderTranspose3, Transpose3);
                
                print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
                print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
                print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
                print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));
                
                print_fix16(renderCounter_t, (counter_t + 1) * 0x00010000);
                
                render_transpose();
                render_counters();
            }
            break;
            
        default:
            break;
    }
}

void step_advance(s32 val) {
    s32 tmp0;
    s32 tmp1;
    s32 tmp2;
    s32 tmp3;
    
    if(val == 0) { return; }
    
    else if (counter < length - 1) {
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
        
        ctl_param_change(eParamTranspose0, Transpose0);
        ctl_param_change(eParamTranspose1, Transpose1);
        ctl_param_change(eParamTranspose2, Transpose2);
        ctl_param_change(eParamTranspose3, Transpose3);
        
        ctl_param_change(eParamFreq0, Freq0);
        ctl_param_change(eParamFreq1, Freq1);
        ctl_param_change(eParamFreq2, Freq2);
        ctl_param_change(eParamFreq3, Freq3);
        
        ctl_param_change(eParamCVfreq0, Freq0);
        ctl_param_change(eParamCVfreq1, Freq1);
        ctl_param_change(eParamCVfreq2, Freq2);
        ctl_param_change(eParamCVfreq3, Freq3);

        print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
        print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
        print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
        print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));
        
        print_fix16(renderCounter, (counter + 1) * 0x00010000);
        
        if (pageIdx != ePagePrgm) { render_freq(); render_counters(); }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
    
    else if (counter < length) {
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
        
        ctl_param_change(eParamTranspose0, Transpose0);
        ctl_param_change(eParamTranspose1, Transpose1);
        ctl_param_change(eParamTranspose2, Transpose2);
        ctl_param_change(eParamTranspose3, Transpose3);

        ctl_param_change(eParamFreq0, Freq0);
        ctl_param_change(eParamFreq1, Freq1);
        ctl_param_change(eParamFreq2, Freq2);
        ctl_param_change(eParamFreq3, Freq3);
        
        ctl_param_change(eParamCVfreq0, Freq0);
        ctl_param_change(eParamCVfreq1, Freq1);
        ctl_param_change(eParamCVfreq2, Freq2);
        ctl_param_change(eParamCVfreq3, Freq3);
        
        print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
        print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
        print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
        print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));
        
        print_fix16(renderCounter, (counter + 1) * 0x00010000);
        
        if (pageIdx != ePagePrgm) { render_freq(); render_counters(); }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

void step_advance_t(s32 val) {
    s32 tmp0;
    s32 tmp1;
    s32 tmp2;
    s32 tmp3;
    
    if(val == 0) { return; }
    
    else if (counter_t < length_t - 1) {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        tmp0 = Transpose0;
        tmp1 = Transpose1;
        tmp2 = Transpose2;
        tmp3 = Transpose3;
        
        step_t[counter_t]->osc0_t = tmp0;
        step_t[counter_t]->osc1_t = tmp1;
        step_t[counter_t]->osc2_t = tmp2;
        step_t[counter_t]->osc3_t = tmp3;
        
        counter_t++;
        
        Transpose0 = step_t[counter_t]->osc0_t;
        Transpose1 = step_t[counter_t]->osc1_t;
        Transpose2 = step_t[counter_t]->osc2_t;
        Transpose3 = step_t[counter_t]->osc3_t;
        
        ctl_param_change(eParamTranspose0, Transpose0);
        ctl_param_change(eParamTranspose1, Transpose1);
        ctl_param_change(eParamTranspose2, Transpose2);
        ctl_param_change(eParamTranspose3, Transpose3);
        
        ctl_param_change(eParamFreq0, Freq0);
        ctl_param_change(eParamFreq1, Freq1);
        ctl_param_change(eParamFreq2, Freq2);
        ctl_param_change(eParamFreq3, Freq3);
        
        ctl_param_change(eParamCVtranspose0, Transpose0);
        ctl_param_change(eParamCVtranspose1, Transpose1);
        ctl_param_change(eParamCVtranspose2, Transpose2);
        ctl_param_change(eParamCVtranspose3, Transpose3);
        
        print_fix16(renderTranspose0, Transpose0);
        print_fix16(renderTranspose1, Transpose1);
        print_fix16(renderTranspose2, Transpose2);
        print_fix16(renderTranspose3, Transpose3);
        
        print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
        print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
        print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
        print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));
        
        print_fix16(renderCounter_t, (counter_t + 1) * 0x00010000);
        
        if (pageIdx != ePagePrgm) { render_transpose(); render_counters(); }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
    
    else if (counter_t < length_t) {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        tmp0 = Transpose0;
        tmp1 = Transpose1;
        tmp2 = Transpose2;
        tmp3 = Transpose3;
        
        step_t[counter_t]->osc0_t = tmp0;
        step_t[counter_t]->osc1_t = tmp1;
        step_t[counter_t]->osc2_t = tmp2;
        step_t[counter_t]->osc3_t = tmp3;
        
        counter_t = 0;
        
        Transpose0 = step_t[counter_t]->osc0_t;
        Transpose1 = step_t[counter_t]->osc1_t;
        Transpose2 = step_t[counter_t]->osc2_t;
        Transpose3 = step_t[counter_t]->osc3_t;
        
        ctl_param_change(eParamTranspose0, Transpose0);
        ctl_param_change(eParamTranspose1, Transpose1);
        ctl_param_change(eParamTranspose2, Transpose2);
        ctl_param_change(eParamTranspose3, Transpose3);
        
        ctl_param_change(eParamFreq0, Freq0);
        ctl_param_change(eParamFreq1, Freq1);
        ctl_param_change(eParamFreq2, Freq2);
        ctl_param_change(eParamFreq3, Freq3);
        
        ctl_param_change(eParamCVtranspose0, Transpose0);
        ctl_param_change(eParamCVtranspose1, Transpose1);
        ctl_param_change(eParamCVtranspose2, Transpose2);
        ctl_param_change(eParamCVtranspose3, Transpose3);
        
        print_fix16(renderTranspose0, Transpose0);
        print_fix16(renderTranspose1, Transpose1);
        print_fix16(renderTranspose2, Transpose2);
        print_fix16(renderTranspose3, Transpose3);

        print_fix16(renderFreq0, fix16_mul(Freq0, Transpose0));
        print_fix16(renderFreq1, fix16_mul(Freq1, Transpose1));
        print_fix16(renderFreq2, fix16_mul(Freq2, Transpose2));
        print_fix16(renderFreq3, fix16_mul(Freq3, Transpose3));
        
        print_fix16(renderCounter_t, (counter_t + 1) * 0x00010000);
        
        if (pageIdx != ePagePrgm) { render_transpose(); render_counters(); }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

void select_tracker(void) {
//assign tracker handlers
    app_event_handlers[ kEventEncoder0 ] = &handle_encoder_3 ;
    app_event_handlers[ kEventEncoder1 ] = &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder2 ] = &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder3 ] = &handle_encoder_0 ;
    app_event_handlers[ kEventSwitch0 ] = &handle_switch_0 ;
    app_event_handlers[ kEventSwitch1 ] = &handle_switch_1 ;
    app_event_handlers[ kEventSwitch2 ]	= &handle_switch_2 ;
    app_event_handlers[ kEventSwitch3 ]	= &handle_switch_3 ;
    app_event_handlers[ kEventSwitch4 ] = &handle_switch_4 ;
}
