//page_level.c
//aleph-prgm-avr32

#include "page_level.h"

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
static etype touched = kNumEventTypes; //total number of module parameters as defined in ctl.h
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

s32 knob_accel(s32 inc) {
    s32 incAbs = inc < 0 ? inc * -1 : inc;
    if(incAbs == 1) {
        return inc;
    }
    if(incAbs < 6) {
        return inc << 2;
    }
    return inc << 6;
}

void handle_switch_0(s32 data) {
    handle_sw(1, data > 0);
}

void handle_switch_1(s32 data) {
    handle_sw(2, data > 0);
}

void handle_switch_2(s32 data) {
    handle_sw(3, data > 0);
    
    if(state_sw == 3) play_step(1);
}

void handle_switch_3(s32 data) {
    handle_sw(4, data > 0);
    
    if(state_sw == 4)
    {
        u8 i;
        gpio_clr_gpio_pin(LED_MODE_PIN);

        counter++;
        
        if (counter < length) i = counter;
        else i = counter = 0;
        
        if (pageIdx != ePageEnv)
        {
            print_fix16(renderL0, track[0]->pL[i]);
            print_fix16(renderL1, track[1]->pL[i]);
            print_fix16(renderL2, track[2]->pL[i]);
            print_fix16(renderL3, track[3]->pL[i]);
            
            print_fix16(renderP0, track[0]->pP[i]);
            print_fix16(renderP1, track[1]->pP[i]);
            print_fix16(renderP2, track[2]->pP[i]);
            print_fix16(renderP3, track[3]->pP[i]);

            print_fix16(renderF0, track[0]->pF[i]);
            print_fix16(renderF1, track[1]->pF[i]);
            print_fix16(renderF2, track[2]->pF[i]);
            print_fix16(renderF3, track[3]->pF[i]);
            
            print_fix16(renderCounter, (i + 1) * 0x00010000);
            
            render_row1();
            render_row2();
            render_row3();
            render_countlev();
        }
        gpio_set_gpio_pin(LED_MODE_PIN);
    }
}

//  switch page
void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        set_page(ePageEnv);
        u8 i = counter;
        
        print_fix16(renderTrig0, track[0]->trig[i]);
        print_fix16(renderTrig1, track[1]->trig[i]);
        print_fix16(renderTrig2, track[2]->trig[i]);
        print_fix16(renderTrig3, track[3]->trig[i]);
        
        print_fix16(renderTime0, track[0]->t[i]);
        print_fix16(renderTime1, track[1]->t[i]);
        print_fix16(renderTime2, track[2]->t[i]);
        print_fix16(renderTime3, track[3]->t[i]);
        
        print_fix16(renderCounter, (i + 1) * 0x00010000);
        
        render_env();
    }
    else ;;
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    u8 i = counter;
    check_touch(kEventEncoder3);
    if (touchedThis) {
        if (get_mode(track[0], i) == 0) ;
        //  HOLD
        else if (get_mode(track[0], i) == 1)
        {
            tmp = track[0]->pF[i];
            tmp += val * 4194304;
            if (tmp < 0) tmp = 0;
            track[0]->pF[i] = tmp;
            ctl_param_change(eParamF0, tmp);
            print_fix16(renderF0, tmp);
            render_row3();
        }
        else if (get_mode(track[0], i) == 2) ;
        else if (get_mode(track[0], i) == 3) ;
        else if (get_mode(track[0], i) == 4) ;
        //  one
        else if (get_mode(track[0], i) == 5)
        {
            tmp = track[0]->pP[i];
            tmp += knob_accel(val);
            if (tmp < 0) tmp = 0;
            track[0]->pP[i] = tmp;
            ctl_param_change(eParamP0, tmp);
            print_fix16(renderP0, tmp);
            render_row2();
        }
        //  loop
        else if (get_mode(track[0], i) == 6)
        {
            tmp = track[0]->pP[i];
            tmp += knob_accel(val);
            if (tmp < 0) tmp = 0;
            track[0]->pP[i] = tmp;
            ctl_param_change(eParamP0, tmp);
            print_fix16(renderP0, tmp);
            render_row2();
        }
        //  scrub
        else if (get_mode(track[0], i) == 7)
        {
            s32 t;
            tmp = track[0]->pP[i];
            t = track[0]->t[i];
            tmp += knob_accel(val);
            t += knob_accel(val);
            if (tmp < 0) tmp = 0;
            if (t < tmp) t = tmp;
            if (t < 0) t++;
            track[0]->pP[i] = tmp;
            track[0]->t[i] = t;
            ctl_param_change(eParamP0, tmp);
            ctl_param_change(eParamTime0, t);
            ctl_param_change(eParamTrig0, 1);
            print_fix16(renderP0, tmp);
            render_row2();
        }
        else if (get_mode(track[0], i) == 8) ;
        else if (get_mode(track[0], i) == 9) ;
        else if (get_mode(track[0], i) == 10) ;
                                     
        else ;
    }
}

void handle_encoder_1(s32 val) {
    s32 tmp;
    u8 i = counter;
    check_touch(kEventEncoder2);
    if (touchedThis) {
        if (get_mode(track[1], i) == 0) ;
        //  HOLD
        else if (get_mode(track[1], i) == 1)
        {
            tmp = track[1]->pF[i];
            tmp += val * 4194304;
            if (tmp < 0) tmp = 0;
            track[1]->pF[i] = tmp;
            ctl_param_change(eParamF1, tmp);
            print_fix16(renderF1, tmp);
            render_row3();
        }
        else if (get_mode(track[1], i) == 2) ;
        else if (get_mode(track[1], i) == 3) ;
        else if (get_mode(track[1], i) == 4) ;
        //  one
        else if (get_mode(track[1], i) == 5)
        {
            tmp = track[1]->pP[i];
            tmp += knob_accel(val);
            if (tmp < 0) tmp = 0;
            track[1]->pP[i] = tmp;
            ctl_param_change(eParamP1, tmp);
            print_fix16(renderP1, tmp);
            render_row2();
        }
        //  loop
        else if (get_mode(track[1], i) == 6)
        {
            tmp = track[1]->pP[i];
            tmp += knob_accel(val);
            if (tmp < 0) tmp = 0;
            track[1]->pP[i] = tmp;
            ctl_param_change(eParamP1, tmp);
            print_fix16(renderP1, tmp);
            render_row2();
        }
        //  scrub
        else if (get_mode(track[1], i) == 7)
        {
            s32 t;
            tmp = track[1]->pP[i];
            t = track[1]->t[i];
            tmp += knob_accel(val);
            t += knob_accel(val);
            if (tmp < 0) tmp = 0;
            if (t < tmp) t = tmp;
            if (t < 0) t++;
            track[1]->pP[i] = tmp;
            track[1]->t[i] = t;
            ctl_param_change(eParamP1, tmp);
            ctl_param_change(eParamTime1, t);
            ctl_param_change(eParamTrig1, 1);
            print_fix16(renderP1, tmp);
            render_row2();
        }
        else if (get_mode(track[1], i) == 8) ;
        else if (get_mode(track[1], i) == 9) ;
        else if (get_mode(track[1], i) == 10) ;
        
        else ;
    }
}

void handle_encoder_2(s32 val) {
    s32 tmp;
    u8 i = counter;
    check_touch(kEventEncoder1);
    if (touchedThis) {
        if (get_mode(track[2], i) == 0) ;
        //  HOLD
        else if (get_mode(track[2], i) == 1)
        {
            tmp = track[2]->pF[i];
            tmp += val * 4194304;
            if (tmp < 0) tmp = 0;
            track[2]->pF[i] = tmp;
            ctl_param_change(eParamF2, tmp);
            print_fix16(renderF2, tmp);
            render_row3();
        }
        else if (get_mode(track[2], i) == 2) ;
        else if (get_mode(track[2], i) == 3) ;
        else if (get_mode(track[2], i) == 4) ;
        //  one
        else if (get_mode(track[2], i) == 5)
        {
            tmp = track[2]->pP[i];
            tmp += knob_accel(val);
            if (tmp < 0) tmp = 0;
            track[2]->pP[i] = tmp;
            ctl_param_change(eParamP2, tmp);
            print_fix16(renderP2, tmp);
            render_row2();
        }
        //  loop
        else if (get_mode(track[2], i) == 6)
        {
            tmp = track[2]->pP[i];
            tmp += knob_accel(val);
            if (tmp < 0) tmp = 0;
            track[2]->pP[i] = tmp;
            ctl_param_change(eParamP2, tmp);
            print_fix16(renderP2, tmp);
            render_row2();
        }
        //  scrub
        else if (get_mode(track[2], i) == 7)
        {
            s32 t;
            tmp = track[2]->pP[i];
            t = track[2]->t[i];
            tmp += knob_accel(val);
            t += knob_accel(val);
            if (tmp < 0) tmp = 0;
            if (t < tmp) t = tmp;
            if (t < 0) t++;
            track[2]->pP[i] = tmp;
            track[2]->t[i] = t;
            ctl_param_change(eParamP2, tmp);
            ctl_param_change(eParamTime2, t);
            ctl_param_change(eParamTrig2, 1);
            print_fix16(renderP2, tmp);
            render_row2();
        }
        else if (get_mode(track[2], i) == 8) ;
        else if (get_mode(track[2], i) == 9) ;
        else if (get_mode(track[2], i) == 10) ;
        
        else ;
    }
}

void handle_encoder_3(s32 val) {
    s32 tmp;
    u8 i = counter;
    check_touch(kEventEncoder0);
    if (touchedThis) {
        if (get_mode(track[3], i) == 0) ;
        //  HOLD
        else if (get_mode(track[3], i) == 1)
        {
            tmp = track[3]->pF[i];
            tmp += val * 4194304;
            if (tmp < 0) tmp = 0;
            track[3]->pF[i] = tmp;
            ctl_param_change(eParamF3, tmp);
            print_fix16(renderF3, tmp);
            render_row3();
        }
        else if (get_mode(track[3], i) == 2) ;
        else if (get_mode(track[3], i) == 3) ;
        else if (get_mode(track[3], i) == 4) ;
        //  one
        else if (get_mode(track[3], i) == 5)
        {
            tmp = track[3]->pP[i];
            tmp += knob_accel(val);
            if (tmp < 0) tmp = 0;
            track[3]->pP[i] = tmp;
            ctl_param_change(eParamP3, tmp);
            print_fix16(renderP3, tmp);
            render_row2();
        }
        //  loop
        else if (get_mode(track[3], i) == 6)
        {
            tmp = track[3]->pP[i];
            tmp += knob_accel(val);
            if (tmp < 0) tmp = 0;
            track[3]->pP[i] = tmp;
            ctl_param_change(eParamP3, tmp);
            print_fix16(renderP3, tmp);
            render_row2();
        }
        //  scrub
        else if (get_mode(track[3], i) == 7)
        {
            s32 t;
            tmp = track[3]->pP[i];
            t = track[3]->t[i];
            tmp += knob_accel(val);
            t += knob_accel(val);
            if (tmp < 0) tmp = 0;
            if (t < tmp) t = tmp;
            if (t < 0) t++;
            track[3]->pP[i] = tmp;
            track[3]->t[i] = t;
            ctl_param_change(eParamP3, tmp);
            ctl_param_change(eParamTime3, t);
            ctl_param_change(eParamTrig3, 1);
            print_fix16(renderP3, tmp);
            render_row2();
        }
        else if (get_mode(track[3], i) == 8) ;
        else if (get_mode(track[3], i) == 9) ;
        else if (get_mode(track[3], i) == 10) ;
        
        else ;
    }
}

/*
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[0]->p1[i];
                tmp += val;
                //  check for lower limit (out of range)
                if (tmp < 0) tmp = 0;
                //  check for upper limit (a bunch of zeroes in the lookup table)
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                track[0]->p1[i] = tmp;
                ctl_param_change(eParamTransposed0, transpose_lookup(tmp));
                print_fix16(renderR0, fix16_mul(track[0]->pFR[i], transpose_lookup(tmp)));
                render_row0();
            }
 
 check_touch(kEventEncoder3);
 if (touchedThis) {
 tmp = track[0]->p2[i];
 tmp += val * 4194304;
 //  set to next source
 if (tmp < 0)
 
 {
 if(i < length)
 tmp = fix16_mul(track[0]->pFR[i + 1], transpose_lookup(track[0]->p1[i + 1]));
 else
 //  if last step, set destination to first step source
 tmp = fix16_mul(track[0]->pFR[0], transpose_lookup(track[0]->p1[0]));
 }
 track[0]->p2[i] = tmp;
 ctl_param_change(eParamCurveDest0, tmp);
 print_fix16(renderD0, tmp);
 render_dest();
 }


void handle_encoder_1(s32 val) {
    s32 tmp;
    u8 i = counter;
//    switch (state_sw) {
//        case 0:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                if (get_mode(track[1], i) == 0) ;
                //  HOLD
                else if (get_mode(track[1], i) == 1)
                {
                    tmp = track[1]->pF[i];
                    tmp += val * 4194304;
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[1]->pF[i] = tmp;
                    ctl_param_change(eParamF1, tmp);
                    print_fix16(renderF1, tmp);
                    render_row3();
                }
                else if (get_mode(track[1], i) == 2) ;
                else if (get_mode(track[1], i) == 3) ;
                //  one
                else if (get_mode(track[1], i) == 4)
                {
                    tmp = track[1]->pP[i];
                    tmp += knob_accel(val);
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[1]->pP[i] = tmp;
                    ctl_param_change(eParamP1, tmp);
                    print_fix16(renderP1, tmp);
                    render_row2();
                }
                //  loop
                else if (get_mode(track[1], i) == 5)
                {
                    tmp = track[1]->pP[i];
                    tmp += knob_accel(val);
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[1]->pP[i] = tmp;
                    ctl_param_change(eParamP1, tmp);
                    print_fix16(renderP1, tmp);
                    render_row2();
                }
                else if (get_mode(track[1], i) == 6) ;
                else if (get_mode(track[1], i) == 7) ;
                else if (get_mode(track[1], i) == 8) ;
                else ;
            }
}

void handle_encoder_2(s32 val) {
    s32 tmp;
    u8 i = counter;
//    switch (state_sw) {
//        case 0:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                if (get_mode(track[2], i) == 0) ;
                //  HOLD
                else if (get_mode(track[2], i) == 1)
                {
                    tmp = track[2]->pF[i];
                    tmp += val * 4194304;
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[2]->pF[i] = tmp;
                    ctl_param_change(eParamF2, tmp);
                    print_fix16(renderF2, tmp);
                    render_row3();
                }
                else if (get_mode(track[2], i) == 2) ;
                else if (get_mode(track[2], i) == 3) ;
                //  one
                else if (get_mode(track[2], i) == 4)
                {
                    tmp = track[2]->pP[i];
                    tmp += knob_accel(val);
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[2]->pP[i] = tmp;
                    ctl_param_change(eParamP2, tmp);
                    print_fix16(renderP2, tmp);
                    render_row2();
                }
                //  loop
                else if (get_mode(track[2], i) == 5)
                {
                    tmp = track[1]->pP[i];
                    tmp += knob_accel(val);
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[1]->pP[i] = tmp;
                    ctl_param_change(eParamP2, tmp);
                    print_fix16(renderP2, tmp);
                    render_row2();
                }
                else if (get_mode(track[2], i) == 6) ;
                else if (get_mode(track[2], i) == 7) ;
                else if (get_mode(track[2], i) == 8) ;
                else ;
            }
}
 
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = track[2]->pFR[i];
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[2]->pFR[i] = tmp;
                ctl_param_change(eParamFract2, tmp);
                print_fix16(renderR2, fix16_mul(tmp, transpose_lookup(track[2]->p1[i])));
                render_row0();
            }
 
 
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = track[2]->p1[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                track[2]->p1[i] = tmp;
                ctl_param_change(eParamTransposed2, transpose_lookup(tmp));
                print_fix16(renderR2, fix16_mul(track[2]->pFR[i], transpose_lookup(tmp)));
                render_row0();
            }
 

void handle_encoder_3(s32 val) {
    s32 tmp;
    u8 i = counter;
//    switch (state_sw) {
//        case 0:
            check_touch(kEventEncoder0);
            if (touchedThis) {
                if (get_mode(track[3], i) == 0) ;
                //  HOLD
                else if (get_mode(track[3], i) == 1)
                {
                    tmp = track[3]->pF[i];
                    tmp += val * 4194304;
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[3]->pF[i] = tmp;
                    ctl_param_change(eParamF3, tmp);
                    print_fix16(renderF3, tmp);
                    render_row3();
                }
                else if (get_mode(track[3], i) == 2) ;
                else if (get_mode(track[3], i) == 3) ;
                //  one
                else if (get_mode(track[3], i) == 4)
                {
                    tmp = track[3]->pP[i];
                    tmp += knob_accel(val);
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[3]->pP[i] = tmp;
                    ctl_param_change(eParamP3, tmp);
                    print_fix16(renderP3, tmp);
                    render_row2();
                }
                //  loop
                else if (get_mode(track[3], i) == 5)
                {
                    tmp = track[3]->pP[i];
                    tmp += knob_accel(val);
                    //  check for lower limit
                    if (tmp < 0) tmp = 0;
                    track[3]->pP[i] = tmp;
                    ctl_param_change(eParamP3, tmp);
                    print_fix16(renderP3, tmp);
                    render_row2();
                }
                else if (get_mode(track[3], i) == 6) ;
                else if (get_mode(track[3], i) == 7) ;
                else if (get_mode(track[3], i) == 8) ;
                else ;
            }
}
 
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[3]->pFR[i];
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[3]->pFR[i] = tmp;
                ctl_param_change(eParamFract3, tmp);
                print_fix16(renderR3, fix16_mul(tmp, transpose_lookup(track[3]->p1[i])));
                render_row0();
            }
 
 
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[3]->p1[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (transpose_lookup(tmp) == 0) { tmp -= val; }
                track[3]->p1[i] = tmp;
                track[3]->psc1[i] = transpose_lookup(tmp);
                ctl_param_change(eParamTransposed3, transpose_lookup(tmp));
                print_fix16(renderR3, fix16_mul(track[3]->pFR[i], transpose_lookup(tmp)));
                render_row0();
            }
 

             check_touch(kEventEncoder0);
             if (touchedThis) {
             tmp = track[3]->p2[i];
             tmp += val * 4194304;
             if (tmp < 0)
             {
             if(i < length)
             tmp = fix16_mul(track[3]->pFR[i + 1], transpose_lookup(track[3]->p1[i + 1]));
             else
             tmp = fix16_mul(track[3]->pFR[0], transpose_lookup(track[3]->p1[0]));
             }
             track[3]->p2[i] = tmp;
             ctl_param_change(eParamCurveDest3, tmp);
             print_fix16(renderD3, tmp);
             render_dest();
             }
*/

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
