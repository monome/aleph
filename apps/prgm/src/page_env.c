//prgm.c
//aleph-prgm-avr32

#include "page_env.h"

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

static s32 time_knob_accel(s32 inc);
static void set_mode(prgmTrack *t, s32 m, u8 offset, u8 i);


//handler variables
static etype touched = kNumEventTypes; //total number as defined in event_types.h
static u8 touchedThis = 0;
static u8 state_sw;

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

s32 time_knob_accel(s32 inc) {
    s32 incAbs = inc < 0 ? inc * -1 : inc;
    if(incAbs == 1) {
        return inc;
    }
    if(incAbs < 6) {
        return inc << 1;
    }
    if(incAbs < 32) {
        return inc << 6;
    }
    return inc << 9;
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
}

void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        u8 i = editpos;
        set_page(ePageParam);
        
        render_track0(track[0]->m[i]);
        render_track1(track[1]->m[i]);
        render_track2(track[2]->m[i]);
        render_track3(track[3]->m[i]);
        
        print_fix16(renderEditPosition, (i + 1 - 16) * 0x00010000);
        
        render_param();
    }
    else ;;
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    u8 i = editpos;
    switch (state_sw) {
        case 0: //motor off: scrub playhead by frame forward | backward*
            check_touch(kEventEncoder3);
            if (touchedThis) {
                //move playhead +/- n samples
            }
            break;
            
        case 1: //curve trig
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[0]->cTG[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                track[0]->cTG[i] = tmp;
                ctl_param_change(i - 16, eParamTrig0, tmp);
                print_fix16(renderTrig0, tmp);
                render_trig();
            }
            break;
            
        case 2: //curve mode
            check_touch(kEventEncoder3);
            if (touchedThis){
                tmp = track[0]->m[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                set_mode(track[0], tmp, 0, i);
                if (tmp == 5) set_mode(track[0], tmp, 0, i);
                if (tmp == 6) set_mode(track[0], tmp, 0, i);
                if (tmp == 7) set_mode(track[0], tmp, 8, i);
                if (tmp == 9) set_mode(track[0], tmp, 0, i);
                if (tmp == 10) set_mode(track[0], tmp, 0, i);
                if (tmp == 13) set_mode(track[0], tmp, 4, i);
                if (tmp == 14) set_mode(track[0], tmp, 4, i);
                ctl_param_change(i - 16, eParamFlag0, track[0]->f[i]);
                ctl_param_change(i - 16, eParamCurve0, track[0]->c[i]);
                ctl_param_change(i - 16, eParamPosition0, track[0]->pP[i]);
                ctl_param_change(i - 16, eParamLoop0, track[0]->pLP[i]);
                render_mode(0, tmp);
                render_trig(); //re-render to cleanup after render_mode()
                render_time(); //re-render to cleanup after render_mode()
            }
            break;
                        
        case 3: //curve time
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[0]->cT[i];
                tmp += time_knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > REC_SIZE_1) tmp = REC_SIZE_1;
                track[0]->cT[i] = tmp;
                ctl_param_change(i - 16, eParamTime0, tmp);
                print_fix16(renderTime0, tmp);
                render_time();
            }
            break;
            
        case 4: //aux level
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[0]->aux;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[0]->aux = tmp;
                ctl_param_change(DUMMY, eParamLevel0, tmp);
                print_fix16(renderLevel0, tmp);
                render_aux_env(0);
            }
            break;
/*
//                s32 frames;
                tmp = measure_lookup;
                tmp += val; //scale lookup
                if (tmp < 0) tmp = 0;
                if (tmp > 5) tmp = 5;
                
                if (tmp == 0)
                {
                    measure = 1;
                    length = 1 + 16;
                    measure_lookup = tmp;
                    ctl_param_change(0, eParamSqLength, length - 16);
                    render_measure_param(tmp);
                }
                
                if (tmp == 1)
                {
                    measure = 2;
                    length = 2 + 16;
                    measure_lookup = tmp;
                    ctl_param_change(0, eParamSqLength, length - 16);
                    render_measure_param(tmp);
                }
                
                if (tmp == 2)
                {
                    measure = 4;
                    length = 4 + 16;
                    measure_lookup = tmp;
                    ctl_param_change(0, eParamSqLength, length - 16);
                    render_measure_param(tmp);
                }
                
                if (tmp == 3)
                {
                    measure = 8;
                    length = 8 + 16;
                    measure_lookup = tmp;
                    ctl_param_change(0, eParamSqLength, length - 16);
                    render_measure_param(tmp);
                }
                
                if (tmp == 4)
                {
                    measure = 16;
                    length = 16 + 16;
                    measure_lookup = tmp;
                    ctl_param_change(0, eParamSqLength, length - 16);
                    render_measure_param(tmp);
                }
                
                if (tmp == 5)
                {
                    measure = 32;
                    length = 32 + 16;
                    measure_lookup = tmp;
                    ctl_param_change(0, eParamSqLength, length - 16);
                    render_measure_param(tmp);
                }
            }
            break;
*/
        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    s32 tmp;
    u8 i = editpos;
    switch (state_sw) {
        case 0: //motor on: nudge playhead slower | faster*
            check_touch(kEventEncoder2);
            if (touchedThis) {
                static event_t e;
                
                if (val)
                {
                    //  trig found
                    e.type = kEventAdc0;
                    e.data = 1;
                    event_post(&e);
                }
            }
            break;
            
        case 1: //curve trig
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[1]->cTG[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                track[1]->cTG[i] = tmp;
                ctl_param_change(i - 16, eParamTrig1, tmp);
                print_fix16(renderTrig1, tmp);
                render_trig();
            }
            break;
            
        case 2: //curve mode
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[1]->m[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                set_mode(track[1], tmp, 0, i);
                if (tmp == 5) set_mode(track[1], tmp, 1, i);
                if (tmp == 6) set_mode(track[1], tmp, 1, i);
                if (tmp == 7) set_mode(track[1], tmp, 8, i);
                if (tmp == 9) set_mode(track[1], tmp, 1, i);
                if (tmp == 10) set_mode(track[1], tmp, 1, i);
                if (tmp == 13) set_mode(track[1], tmp, 5, i);
                if (tmp == 14) set_mode(track[1], tmp, 5, i);
                ctl_param_change(i - 16, eParamFlag1, track[1]->f[i]);
                ctl_param_change(i - 16, eParamCurve1, track[1]->c[i]);
                ctl_param_change(i - 16, eParamPosition1, track[1]->pP[i]);
                ctl_param_change(i - 16, eParamLoop1, track[1]->pLP[i]);
                render_mode(1, tmp);
                render_trig(); //re-render to cleanup after render_mode()
                render_time(); //re-render to cleanup after render_mode()
            }
            break;
            
        case 3: //curve time
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[1]->cT[i];
                tmp += time_knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > REC_SIZE_1) tmp = REC_SIZE_1;
                track[1]->cT[i] = tmp;
                ctl_param_change(i - 16, eParamTime1, tmp);
                print_fix16(renderTime1, tmp);
                render_time();
            }
            break;

        case 4: //aux level
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[1]->aux;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[1]->aux = tmp;
                ctl_param_change(DUMMY, eParamLevel1, tmp);
                print_fix16(renderLevel1, tmp);
                render_aux_env(1);
            }
            break;
/*
//                u8 j;
//                s32 frames;
                tmp = tempo_lookup;
                tmp += val; //scale lookup
                if (tmp < 0) tmp = 0;
                if (tmp > 15) tmp = 15;
                tempo_lookup = tmp;
                tempo = clockspeed_lookup(tmp);
                //recalculate step length to frames and update bfin

                for (j=0; j<length; j++)
                {
                    frames = n_scale[j] * tempo * FRAMES;
                    ctl_param_change(j, eParamFrames, frames);
                }

                print_fix16(renderTempo, tempo * 0x00010000);
                render_tempo_env();
            }
            break;
*/
        default:
            break;
    }
}

void handle_encoder_2(s32 val) {
    s32 tmp;
    u8 i = editpos;
    switch (state_sw) {
        case 0: //move playhead to edit position & play this step forward | backward
            check_touch(kEventEncoder1);
            if (touchedThis) {
                ctl_param_change(0, eParamSqCounter, i - 16);
            }
            break;

        case 1: //curve trig
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = track[2]->cTG[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                track[2]->cTG[i] = tmp;
                ctl_param_change(i - 16, eParamTrig2, tmp);
                print_fix16(renderTrig2, tmp);
                render_trig();
            }
            break;
            
        case 2: //curve mode
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = track[2]->m[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                set_mode(track[2], tmp, 0, i);
                if (tmp == 5) set_mode(track[2], tmp, 2, i);
                if (tmp == 6) set_mode(track[2], tmp, 2, i);
                if (tmp == 7) set_mode(track[2], tmp, 8, i);
                if (tmp == 9) set_mode(track[2], tmp, 2, i);
                if (tmp == 10) set_mode(track[2], tmp, 2, i);
                if (tmp == 13) set_mode(track[2], tmp, 6, i);
                if (tmp == 14) set_mode(track[2], tmp, 6, i);
                ctl_param_change(i - 16, eParamFlag2, track[2]->f[i]);
                ctl_param_change(i - 16, eParamCurve2, track[2]->c[i]);
                ctl_param_change(i - 16, eParamPosition2, track[2]->pP[i]);
                ctl_param_change(i - 16, eParamLoop2, track[2]->pLP[i]);
                render_mode(2, tmp);
                render_trig(); //re-render to cleanup after render_mode()
                render_time(); //re-render to cleanup after render_mode()
            }
            break;
            
        case 3: //curve time
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = track[2]->cT[i];
                tmp += time_knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > REC_SIZE_1) tmp = REC_SIZE_1;
                track[2]->cT[i] = tmp;
                ctl_param_change(i - 16, eParamTime2, tmp);
                print_fix16(renderTime2, tmp);
                render_time();
            }
            break;
            
        case 4: //aux level
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = track[2]->aux;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[2]->aux = tmp;
                ctl_param_change(DUMMY, eParamLevel2, tmp);
                print_fix16(renderLevel2, tmp);
                render_aux_env(2);
            }
            break;
/*
//                s32 frames;
                tmp = n_scale_lookup[i];
                tmp += val; //scale lookup
                if (tmp < 0) { tmp = 0; n_scale[i] = straight_lookup(tmp); }
                if (tmp > 0 && tmp < 7) { n_scale[i] = straight_lookup(tmp); }
                //align step length with measure
                if (tmp > 7) { tmp = 7; n_scale[i] = measure; }
                //recalculate step length to frames and update bfin
                n_scale_lookup[i] = tmp;
//                frames = n_scale[i] * tempo * FRAMES;
//                ctl_param_change(i - 16, eParamFrames, frames);
                print_fix16(renderStepLength, tmp);
                render_steplength_env(i);
                //                render_counters_env();
            }
            break;
*/
        default:
            break;
    }
}

void handle_encoder_3(s32 val) {
    s32 tmp;
    s8 i = editpos;
    switch (state_sw) {
        case 0: //set edit position
            check_touch(kEventEncoder0);
            if (touchedThis) {
                i += val;
                if (i < 16) i = length - 1;
                if (i > length - 1) i = 16;
                editpos = i;
                
                print_fix16(renderTrig0, track[0]->cTG[i]);
                print_fix16(renderTrig1, track[1]->cTG[i]);
                print_fix16(renderTrig2, track[2]->cTG[i]);
                print_fix16(renderTrig3, track[3]->cTG[i]);
                
                print_fix16(renderTime0, track[0]->cT[i]);
                print_fix16(renderTime1, track[1]->cT[i]);
                print_fix16(renderTime2, track[2]->cT[i]);
                print_fix16(renderTime3, track[3]->cT[i]);

                print_fix16(renderEditPosition, (i + 1 - 16) * 0x00010000);
                
                render_modes(i);
                render_trig();
                render_time();
                render_counters_env();
            }
            break;
            
        case 1: //curve trig
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[3]->cTG[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 1) tmp = 1;
                track[3]->cTG[i] = tmp;
                ctl_param_change(i - 16, eParamTrig3, tmp);
                print_fix16(renderTrig3, tmp);
                render_trig();
            }
            break;
            
        case 2: //curve mode
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[3]->m[i];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                set_mode(track[3], tmp, 0, i);
                if (tmp == 5) set_mode(track[3], tmp, 3, i);
                if (tmp == 6) set_mode(track[3], tmp, 3, i);
                if (tmp == 7) set_mode(track[3], tmp, 8, i);
                if (tmp == 9) set_mode(track[3], tmp, 3, i);
                if (tmp == 10) set_mode(track[3], tmp, 3, i);
                if (tmp == 13) set_mode(track[3], tmp, 7, i);
                if (tmp == 14) set_mode(track[3], tmp, 7, i);
                ctl_param_change(i - 16, eParamFlag3, track[3]->f[i]);
                ctl_param_change(i - 16, eParamCurve3, track[3]->c[i]);
                ctl_param_change(i - 16, eParamPosition3, track[3]->pP[i]);
                ctl_param_change(i - 16, eParamLoop3, track[3]->pLP[i]);
                render_mode(3, tmp);
                render_trig(); //re-render to cleanup after render_mode()
                render_time(); //re-render to cleanup after render_mode()
            }
            break;
            
        case 3: //curve time
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[3]->cT[i];
                tmp += time_knob_accel(val);
                if (tmp < 0) tmp = 0;
                if (tmp > REC_SIZE_1) tmp = REC_SIZE_1;
                track[3]->cT[i] = tmp;
                ctl_param_change(i - 16, eParamTime3, tmp);
                print_fix16(renderTime3, tmp);
                render_time();
            }
            break;

        case 4: //aux level
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[3]->aux;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[3]->aux = tmp;
                ctl_param_change(DUMMY, eParamLevel3, tmp);
                print_fix16(renderLevel3, tmp);
                render_aux_env(3);
            }
            break;
/*
                tmp = bufferpos;
                tmp += val;
                if (tmp < 0) tmp = N_BUFFERS_1;
                if (tmp > N_BUFFERS_1) tmp = 0;
                bufferpos = tmp;
                //  set mode to recIn1 on bfin, and buffer boundaries to selected track
                if(tmp == 0)
                {
                    //Packed = FLAG : CURVE : TRACK : TRIG, also use Packed for trig curve when changing postion in mode one, wav and trig sample when selecting wav
                    //                    ctl_param_change(0, eParamPacked, buffer.asS32);
                    //                    ctl_param_change(0, eParamBufferPosition0, offset);
                    //                    ctl_param_change(0, eParamBufferLength0, offset);
                }
                
                else if(tmp == 1)
                {
                    
                }

                else if(tmp == 2)
                {
                    
                }
                
                else if(tmp == 3)
                {
                    
                }
                
                //  render selected buffer/track
//                print_fix16(renderBufferPosition, (tmp + 1) * 0x00010000);
//                render_bufferposition_env(tmp);
            }
            break;
*/
        default:
            break;
    }
}

void set_mode(prgmTrack *t, s32 m, u8 offset, u8 i) {
    //  off
    if(m == 0)
    {
        t->m[i] = 0;
        t->f[i] = 0;
        t->c[i] = 0;
        t->pP[i] = offset;
    }
    
    //  HOLD
    else if(m == 1)
    {
        t->m[i] = 1;
        t->f[i] = 7;
        t->c[i] = 1;
        t->pP[i] = offset;
    }
    
    //  TRIG
    else if(m == 2)
    {
        t->m[i] = 2;
        t->f[i] = 6;
        t->c[i] = 2;
        t->pP[i] = offset;
    }
    
    //  GATE
    else if(m == 3)
    {
        t->m[i] = 3;
        t->f[i] = 6;
        t->c[i] = 3;
        t->pP[i] = offset;
    }
    
    //  NOISE
    else if(m == 4)
    {
        t->m[i] = 4;
        t->f[i] = 6;
        t->c[i] = 6;
        t->pP[i] = offset;
    }
    
    //  one
    else if(m == 5)
    {
        t->m[i] = 5;
        t->f[i] = 2;
        t->c[i] = 4;
        t->pP[i] = sample[offset]->offset;
        t->pLP[i] = sample[offset]->size;
    }
    
    //  loop
    else if(m == 6)
    {
        t->m[i] = 6;
        t->f[i] = 2;
        t->c[i] = 5;
        t->pP[i] = sample[offset]->offset;
        t->pLP[i] = sample[offset]->size;
    }
    
    //  wav
    else if(m == 7)
    {
        t->m[i] = 7;
        t->f[i] = 2;
        t->c[i] = 4;
        t->pP[i] = sample[offset]->offset;
        t->pLP[i] = sample[offset]->size;
    }
    
    //  noise
    else if(m == 8)
    {
        t->m[i] = 8;
        t->f[i] = 1;
        t->c[i] = 6;
        t->pP[i] = offset;
    }
    
    //  rec
    else if(m == 9)
    {
        t->m[i] = 9;
        t->f[i] = 1;
        t->c[i] = 7;
        t->pP[i] = sample[offset]->offset;
        t->pLP[i] = sample[offset]->size;
    }
    
    //  TRIGrec
    else if(m == 10)
    {
        t->m[i] = 10;
        t->f[i] = 6;
        t->c[i] = 8;
        t->pP[i] = sample[offset]->offset;
        t->pLP[i] = sample[offset]->size;
    }
    
    //  [aux master]
    else if(m == 11)
    {
        t->m[i] = 11;
        t->f[i] = 1;
        t->c[i] = 9;
        t->pP[i] = offset;
    }
    
    //  [insert mix]
    else if(m == 12)
    {
        t->m[i] = 12;
        t->f[i] = 4;
        t->c[i] = 10;
        t->pP[i] = offset;
    }

    //  [delay]
    else if(m == 13)
    {
        t->m[i] = 13;
        t->f[i] = 3;
        t->c[i] = 11;
        t->pP[i] = sample[offset]->offset;
        t->pLP[i] = sample[offset]->size;
    }
    
    //  [diffuse]
    else if(m == 14)
    {
        t->m[i] = 14;
        t->f[i] = 3;
        t->c[i] = 12;
        t->pP[i] = sample[offset]->offset;
        t->pLP[i] = sample[offset]->size;
    }
    
    //  <GLOBAL>
    else if(m == 15)
    {
        t->m[i] = 15;
        t->f[i] = 0;
        t->c[i] = 0;
        t->pP[i] = offset;
    }
    
    else ;
}

void select_env(void) {
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
