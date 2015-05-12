//page_param.c
//aleph-prgm-avr32

#include "page_cut.h"

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

static s32 knob_accel(s32 inc);


//handler variables
static etype touched = kNumEventTypes;
static u8 touchedThis = 0;
static u8 state_sw;

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

s32 knob_accel(s32 inc) {
    s32 incAbs = inc < 0 ? inc * -1 : inc;
    if(incAbs < 2) {
        return inc;
    }
    if(incAbs < 8) {
        return inc << 1;
    }
    if(incAbs < 32) {
        return inc << 4;
    }
    return inc << 8;
}

void handle_switch_0(s32 data) {
    handle_sw(1, data > 0);
}

void handle_switch_1(s32 data) {
    handle_sw(2, data > 0);
}

void handle_switch_2(s32 data) {
    handle_sw(3, data > 0);
    
    if(state_sw == 3)
    {
        set_page(ePageMix);
        render_mix();
        handle_switch_0(0);
    }
    else ;;
}

void handle_switch_3(s32 data) {
    handle_sw(4, data > 0);
}

void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    const u8 s = samplepos;
    switch (state_sw) {
        case 0:
            //  offset cut
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = sample[s]->offset_cut;
                tmp += knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > sample[s]->loop + (sample[s]->loop_cut) - LOOP_MIN) tmp = sample[s]->loop + (sample[s]->loop_cut) - LOOP_MIN;
                sample[s]->offset_cut = tmp;
                render_cut();
            }
            break;
                    
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
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
    const u8 s = samplepos;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                //  loop cut
                tmp = sample[s]->loop_cut;
                tmp += knob_accel(val);
                //  sample length boundary
                if (sample[s]->loop + (tmp) > sample[s]->loop) tmp = 0;
                //  loop length boundary
                if (sample[s]->loop + (tmp) < sample[s]->offset_cut + LOOP_MIN) tmp = -(sample[s]->loop - (sample[s]->offset_cut + LOOP_MIN));
                sample[s]->loop_cut = tmp;
                render_cut();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
            }
            break;
            
        case 2:
            check_touch(kEventEncoder2);
            if (touchedThis) {
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
            //  offset cut
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
            //  sample position
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = samplepos;
                tmp += val;
                if (tmp < N_BUFFERS + 1) tmp = N_BUFFERS + 1;
                if (tmp > n_samples - 1) tmp = n_samples - 1;
                samplepos = tmp;
                render_cut();
            }
            break;
            
        case 1:
            check_touch(kEventEncoder0);
            if (touchedThis) {
            }
            break;
            
        case 2:
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

void select_cut(void) {
//assign tracker handlers
    app_event_handlers[ kEventEncoder0 ] = &handle_encoder_0 ;
    app_event_handlers[ kEventEncoder1 ] = &handle_encoder_1 ;
    app_event_handlers[ kEventEncoder2 ] = &handle_encoder_2 ;
    app_event_handlers[ kEventEncoder3 ] = &handle_encoder_3 ;
    app_event_handlers[ kEventSwitch0 ] = &handle_switch_0 ;
    app_event_handlers[ kEventSwitch1 ] = &handle_switch_1 ;
    app_event_handlers[ kEventSwitch2 ] = &handle_switch_2 ;
    app_event_handlers[ kEventSwitch3 ] = &handle_switch_3 ;
    app_event_handlers[ kEventSwitch4 ] = &handle_switch_4 ;
}
