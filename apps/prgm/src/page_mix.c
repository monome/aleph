//prgm.c
//aleph-prgm-avr32

#include "page_mix.h"

//static functions
static inline void handle_sw(u8 id, u8 b);
static void handle_switch_0(s32 data);
static void handle_switch_1(s32 data);
static void handle_switch_2(s32 data);
static void handle_switch_3(s32 data);
static void handle_switch_4(s32 data);

static void handle_encoder_0(s32 val);
static void handle_encoder_1(s32 val);
static void handle_encoder_2(s32 val);
static void handle_encoder_3(s32 val);


//handler variables
static etype touched = kNumEventTypes; //total number as defined in event_types.h
static u8 touchedThis = 0;
static u8 state_sw;
static event_t e;

static inline u8 check_touch(etype et) {
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
    handle_sw(3, data > 0);
}

void handle_switch_3(s32 data) {
    handle_sw(4, data > 0);
    
    //  return all counters tp zero?!
}

void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        gpio_clr_gpio_pin(LED_MODE_PIN);
        e.type = kEventAdc0;
        e.data = 1;
        event_post(&e);

        delay_ms(55);
        gpio_set_gpio_pin(LED_MODE_PIN);
        
        ctl_param_change(DUMMY, eParamCounter, 0);
    }
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = master->pan1;
                tmp += val * 4194304;
                master->pan1 = tmp;
                ctl_param_change(DUMMY, eParamAux1PanL, sub_fr1x32(FR32_MAX, tmp));
                ctl_param_change(DUMMY, eParamAux1PanR, tmp);
                render_mix();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = master->pan2;
                tmp += val * 4194304;
                master->pan2 = tmp;
                ctl_param_change(DUMMY, eParamAux2PanL, sub_fr1x32(FR32_MAX, tmp));
                ctl_param_change(DUMMY, eParamAux2PanR, tmp);
                render_mix();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder3);
            if (touchedThis) {
            }
            break;
            
        case 3:
            check_touch(kEventEncoder3);
            if (touchedThis) {
            }
            break;
            
        case 4:
            check_touch(kEventEncoder3);
            if (touchedThis) {
            }
            break;

        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = master->output;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                master->output = tmp;
                ctl_param_change(DUMMY, eParamMaster, tmp);
                render_mix();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = master->out3;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_DIROUTS_1) tmp = N_DIROUTS_1;
                master->out3 = tmp;
                ctl_param_change(DUMMY, eParamDirectOut3, tmp);
                render_mix();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                //aux2 level?!?
            }
            break;
            
        case 3:
            check_touch(kEventEncoder2);
            if (touchedThis) {
            }
            break;
            
        case 4:
            check_touch(kEventEncoder2);
            if (touchedThis) {
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
            }
            break;

        case 1:
            check_touch(kEventEncoder1);
            if (touchedThis) {
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
            }
            break;
            
        case 3:
            check_touch(kEventEncoder1);
            if (touchedThis) {
            }
            break;

        case 4:
            check_touch(kEventEncoder1);
            if (touchedThis) {
            }
            break;
            
        default:
            break;
    }
}

void handle_encoder_3(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = 0;
                tmp += val;
                if (tmp < 0)
                {
                    tmp = 0;
                    set_page(ePageTrk);
                    render_trk();
                    break;
                }
                if (tmp > 0)
                {
                    tmp = 0;
                    set_page(ePageMix);
                    render_mix();
                    handle_switch_3(0);
                    break;
                }
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = master->out4;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_DIROUTS_1) tmp = N_DIROUTS_1;
                master->out4 = tmp;
                ctl_param_change(DUMMY, eParamDirectOut4, tmp);
                render_mix();
            }
            break;
            
        case 2:
            check_touch(kEventEncoder0);
            if (touchedThis) {
            }
            break;

        case 3:
            check_touch(kEventEncoder0);
            if (touchedThis) {
            }
            break;

        case 4:
            check_touch(kEventEncoder0);
            if (touchedThis) {
            }
            break;

        default:
            break;
    }
}

void select_mix(void) {
//assign prgm handlers
    app_event_handlers[ kEventEncoder0 ]	= &handle_encoder_0 ;
    app_event_handlers[ kEventEncoder1 ]	= &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder2 ]	= &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder3 ]	= &handle_encoder_3 ;
    app_event_handlers[ kEventSwitch0 ]     = &handle_switch_0 ;
    app_event_handlers[ kEventSwitch1 ]     = &handle_switch_1 ;
    app_event_handlers[ kEventSwitch2 ]     = &handle_switch_2 ;
    app_event_handlers[ kEventSwitch3 ]     = &handle_switch_3 ;
    app_event_handlers[ kEventSwitch4 ]     = &handle_switch_4 ;
}
