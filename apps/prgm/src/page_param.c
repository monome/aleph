//page_param.c
//aleph-prgm-avr32

#include "page_param.h"

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
static s32 knob_deccel(s32 inc);


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

//flip page
void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        set_page(ePageEnv);
        u8 i = editpos;
        
        print_fix16(renderTrig0, track[0]->cTG[i]);
        print_fix16(renderTrig1, track[1]->cTG[i]);
        print_fix16(renderTrig2, track[2]->cTG[i]);
        print_fix16(renderTrig3, track[3]->cTG[i]);
        
        print_fix16(renderTime0, track[0]->cT[i]);
        print_fix16(renderTime1, track[1]->cT[i]);
        print_fix16(renderTime2, track[2]->cT[i]);
        print_fix16(renderTime3, track[3]->cT[i]);
        
        print_fix16(renderStepLength, n_scale[i] * 0x00010000);
        print_fix16(renderEditPosition, (i + 1 - 16) * 0x00010000);
        
        render_env();
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
        
        case 1: //mode parameter 1
            check_touch(kEventEncoder3);
            if (touchedThis) {
                
                //off
                if (track[0]->m[i] == 0) ;
                
                //HOLD: cv output level
                else if (track[0]->m[i] == 1)
                {
                    tmp = track[0]->pF[i];
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[0]->pF[i] = tmp;
                    ctl_param_change(i - 16, eParamFrequency0, tmp);
                    render_track0(1);
                }
                
                /*
                 {
                 tmp = track[0]->pF[i];
                 tmp += val;
                 if (tmp < 0) tmp = 0;
                 if (tmp > 1024) tmp = 1024;
                 track[0]->pF[i] = tmp;
                 track[0]->pF_scale[i] = just_lookup(tmp);
                 ctl_param_change(i - 16, eParamFrequency0, track[0]->pF_scale[i]);
                 print_fix16(renderFrequency0, tmp);
                 render_track0(11);
                 }
                */
                
                //TRIG
                else if (track[0]->m[i] == 2) ;
                
                //GATE
                else if (track[0]->m[i] == 3) ;
                
                //NOISE
                else if (track[0]->m[i] == 4) ;

                //one: recording buffer position
                else if (track[0]->m[i] == 5)
                {
                    tmp = track[0]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < 0) tmp = 0;
                    if (tmp > sample[1]->offset) tmp = sample[1]->offset;
                    track[0]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition0, tmp);
                    render_track0(5);
                }
                
                //loop: recording buffer position
                else if (track[0]->m[i] == 6)
                {
                    tmp = track[0]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < 0) tmp = 0;
                    if (tmp > sample[1]->offset) tmp = sample[1]->offset;
                    track[0]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition0, tmp);
                    render_track0(6);
                }
                
                //wav: sample
                else if (track[0]->m[i] == 7)
                {
                    tmp = track[0]->pS[i];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[0]->pS[i] = tmp;
                    track[0]->pP[i] = sample[tmp]->offset;
                    track[0]->pLP[i] = sample[tmp]->size - 1;
                    ctl_param_change(i - 16, eParamPosition0, track[0]->pP[i]);
                    ctl_param_change(i - 16, eParamLoop0, track[0]->pLP[i]);
                    //  set step length equal to sample length for now... TEST!!!!!!
                    //ctl_param_change(i - 16, eParamTime0, track[0]->pLP[i]);
                    render_track0(7);
                }
                
                //noise
                else if (track[0]->m[i] == 8) ;
                
                //rec: input A
                else if (track[0]->m[i] == 9) {
                    tmp = track[0]->inA;
                    tmp += knob_accel(val); //TEST!!!
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[0]->inA = tmp;
                    ctl_param_change(i - 16, eParamInputA0, tmp);
                    render_track0(9);
                }
                
                //TRIGrec: physical input A
                else if (track[0]->m[i] == 10) {
                    tmp = track[0]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_PHYSICAL_INPUTS_1) tmp = N_PHYSICAL_INPUTS_1;
                    track[0]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA0, tmp);
                    render_track0(10);
                }
            
                //[aux master]
                else if (track[0]->m[i] == 11) ;
                
                //[insert mix]: input A
                else if (track[0]->m[i] == 12) {
                    tmp = track[0]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[0]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA0, tmp);
                    render_track0(12);
                }
                
                //[delay]: input A
                else if (track[0]->m[i] == 13) {
                    tmp = track[0]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[0]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA0, tmp);
                    render_track0(13);
                }
                
                //[diffuse]: input A
                else if (track[0]->m[i] == 14) {
                    tmp = track[0]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[0]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA0, tmp);
                    render_track0(14);
                }
                
                //<GLOBAL>: sq length
                else if (track[0]->m[i] == 15)
                {
                    tmp = length;
                    tmp += val;
                    if (tmp < 1 + 16) tmp = 1 + 16;
                    if (tmp > SQ_LEN) tmp = SQ_LEN;
                    length = tmp;
                    ctl_param_change(DUMMY, eParamSqLength, length - 16);
                    render_track0(15);
                }
                
                else ;
            }
            break;
            
        case 2: //mode parameter 2
            check_touch(kEventEncoder3);
            if (touchedThis) {
                
                //off
                if (track[0]->m[i] == 0) ;
                
                //HOLD:
                else if (track[0]->m[i] == 1) ;
                
                //TRIG
                else if (track[0]->m[i] == 2) ;
                
                //GATE
                else if (track[0]->m[i] == 3) ;
                
                //NOISE
                else if (track[0]->m[i] == 4) ;
                
                //one:
                else if (track[0]->m[i] == 5) ;
                
                //loop:
                else if (track[0]->m[i] == 6) ;
                
                //wav: sample start position
                else if (track[0]->m[i] == 7)
                {
                    tmp = track[0]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[track[0]->pS[i]]->offset) tmp = sample[track[0]->pS[i]]->offset;
                    if (tmp > sample[track[0]->pS[i] + 1]->offset) tmp = sample[track[0]->pS[i] + 1]->offset;
                    track[0]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition0, tmp);
                    render_track0(7);
                }
                
                //noise
                else if (track[0]->m[i] == 8) ;
                
                //rec
                else if (track[0]->m[i] == 9) ;

                //TRIGrec
                else if (track[0]->m[i] == 10) ;
                
                //[aux master]
                else if (track[0]->m[i] == 11) ;
                
                //[insert mix]: input B
                else if (track[0]->m[i] == 12) {
                    tmp = track[0]->inB;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[0]->inB = tmp;
                    ctl_param_change(DUMMY, eParamInputB0, tmp);
                    render_track0(12);
                }
                
                //[delay]: loop
                else if (track[0]->m[i] == 13)
                {
                    tmp = track[0]->pLP[i];
                    tmp += knob_accel(val);
                    //  a zero loop time will freeze the dsp...
                    if (tmp < 1) tmp = 256;
                    if (tmp > AUX_SIZE_1) tmp = AUX_SIZE_1;
                    track[0]->pLP[i] = tmp;
                    ctl_param_change(i - 16, eParamLoop0, tmp);
                    render_track0(13);
                }
                
                //[diffuse]: time
                else if (track[0]->m[i] == 14) {
                    tmp = track[0]->pLP[i];
                    tmp += knob_accel(val);
                    //  a zero loop time will freeze the dsp...
                    if (tmp < 1) tmp = 256;
                    if (tmp > AUX_SIZE_1) tmp = AUX_SIZE_1;
                    track[0]->pLP[i] = tmp;
                    ctl_param_change(i - 16, eParamLoop0, tmp);
                    render_track0(14);
                }

                else ;
            }
            break;
            
        case 3: //mode parameter 3
            check_touch(kEventEncoder3);
            if (touchedThis) {
                
                //off
                if (track[0]->m[i] == 0) ;
                
                //HOLD:
                else if (track[0]->m[i] == 1) ;
                
                //TRIG
                else if (track[0]->m[i] == 2) ;
                
                //GATE
                else if (track[0]->m[i] == 3) ;
                
                //NOISE
                else if (track[0]->m[i] == 4) ;
                
                //one:
                else if (track[0]->m[i] == 5) ;
                
                //loop
                else if (track[0]->m[i] == 6) ;
                
                //wav
                else if (track[0]->m[i] == 7) ;
                
                //noise
                else if (track[0]->m[i] == 8) ;
                
                //rec
                else if (track[0]->m[i] == 9) ;
                
                //TRIGrec
                else if (track[0]->m[i] == 10) ;
                
                //[aux master]
                else if (track[0]->m[i] == 11) ;
                
                //[insert mix]: mix
                else if (track[0]->m[i] == 12) {
                    tmp = track[0]->mix;
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[0]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix0, tmp);
                    render_track0(12);
                }
                
                //[delay]: mix
                else if (track[0]->m[i] == 13) {
                    tmp = track[0]->mix;
                    tmp += val * 4194304;
                    track[0]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix0, tmp);
                    render_track0(13);
                }
                
                //[diffuse]: mix
                else if (track[0]->m[i] == 14) {
                    tmp = track[0]->mix;
                    tmp += val * 4194304;
                    track[0]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix0, tmp);
                    render_track0(14);
                }

                else ;
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
                render_aux_param(0);
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
            
        case 1: //mode parameter 1
            check_touch(kEventEncoder2);
            if (touchedThis) {
                
                //off
                if (track[1]->m[i] == 0) ;
                
                //HOLD: cv output level
                else if (track[1]->m[i] == 1)
                {
                    tmp = track[1]->pF[i];
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[1]->pF[i] = tmp;
                    ctl_param_change(i - 16, eParamFrequency1, tmp);
                    render_track1(1);
                }
                
                //TRIG
                else if (track[1]->m[i] == 2) ;
                
                //GATE
                else if (track[1]->m[i] == 3) ;
                
                //NOISE
                else if (track[1]->m[i] == 4) ;
                
                //one: recording buffer position
                else if (track[1]->m[i] == 5)
                {
                    tmp = track[1]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[1]->offset) tmp = sample[1]->offset;
                    if (tmp > sample[2]->offset) tmp = sample[2]->offset;
                    track[1]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition1, tmp);
                    render_track1(5);
                }
                
                //loop: recording buffer position
                else if (track[1]->m[i] == 6)
                {
                    tmp = track[1]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[1]->offset) tmp = sample[1]->offset;
                    if (tmp > sample[2]->offset) tmp = sample[2]->offset;
                    track[1]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition1, tmp);
                    render_track1(6);
                }
                
                //wav: sample
                else if (track[1]->m[i] == 7)
                {
                    tmp = track[1]->pS[i];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[1]->pS[i] = tmp;
                    track[1]->pP[i] = sample[tmp]->offset;
                    track[1]->pLP[i] = sample[tmp]->size - 1;
                    ctl_param_change(i - 16, eParamPosition1, track[1]->pP[i]);
                    ctl_param_change(i - 16, eParamLoop1, track[1]->pLP[i]);
                    render_track1(7);
                }
                
                //noise
                else if (track[1]->m[i] == 8) ;
                
                //rec: input A
                else if (track[1]->m[i] == 9) {
                    tmp = track[1]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[1]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA1, tmp);
                    render_track1(9);
                }

                //TRIGrec: physical input A
                else if (track[1]->m[i] == 10) {
                    tmp = track[1]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_PHYSICAL_INPUTS_1) tmp = N_PHYSICAL_INPUTS_1;
                    track[1]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA1, tmp);
                    render_track1(10);
                }
            
                //[aux master]
                else if (track[1]->m[i] == 11) ;
                
                //[insert mix]: input A
                else if (track[1]->m[i] == 12) {
                    tmp = track[1]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[1]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA1, tmp);
                    render_track1(12);
                }
                
                //[delay]: input A
                else if (track[1]->m[i] == 13) {
                    tmp = track[1]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[1]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA1, tmp);
                    render_track1(13);
                }
                
                //[diffuse]: input A
                else if (track[1]->m[i] == 14) {
                    tmp = track[1]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[1]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA1, tmp);
                    render_track1(14);
                }
                
                //<GLOBAL>: sq length
                else if (track[1]->m[i] == 15)
                {
                    tmp = length;
                    tmp += val;
                    if (tmp < 1 + 16) tmp = 1 + 16;
                    if (tmp > SQ_LEN) tmp = SQ_LEN;
                    length = tmp;
                    ctl_param_change(DUMMY, eParamSqLength, length - 16);
                    render_track1(15);
                }

                else ;
            }
            break;
            
        case 2: //mode parameter 2
            check_touch(kEventEncoder2);
            if (touchedThis) {
                
                //off
                if (track[1]->m[i] == 0) ;
                
                //HOLD:
                else if (track[1]->m[i] == 1) ;
                
                //TRIG
                else if (track[1]->m[i] == 2) ;
                
                //GATE
                else if (track[1]->m[i] == 3) ;
                
                //NOISE
                else if (track[1]->m[i] == 4) ;
                
                //one:
                else if (track[1]->m[i] == 5) ;
                
                //loop:
                else if (track[1]->m[i] == 6) ;
                
                //wav: sample start position
                else if (track[1]->m[i] == 7)
                {
                    tmp = track[1]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[track[1]->pS[i]]->offset) tmp = sample[track[1]->pS[i]]->offset;
                    if (tmp > sample[track[1]->pS[i] + 1]->offset) tmp = sample[track[1]->pS[i] + 1]->offset;
                    track[1]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition1, tmp);
                    render_track1(7);
                }
                
                //noise
                else if (track[1]->m[i] == 8) ;
                
                //rec
                else if (track[1]->m[i] == 9) ;
                
                //TGrecin2
                else if (track[1]->m[i] == 10) ;

                //[aux master]
                else if (track[1]->m[i] == 11) ;
                
                //[insert mix]: input B
                else if (track[1]->m[i] == 12) {
                    tmp = track[1]->inB;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[1]->inB = tmp;
                    ctl_param_change(i - 16, eParamInputB1, tmp);
                    render_track1(12);
                }
                
                //[delay]: loop
                else if (track[1]->m[i] == 13) {
                    tmp = track[1]->pLP[i];
                    tmp += knob_accel(val);
                    //  a zero loop time will freeze the dsp...
                    if (tmp < 1) tmp = 256;
                    if (tmp > AUX_SIZE_1) tmp = AUX_SIZE_1;
                    track[1]->pLP[i] = tmp;
                    ctl_param_change(i - 16, eParamLoop1, tmp);
                    render_track1(13);
                }
                
                //[diffuse]: time
                else if (track[1]->m[i] == 14) {
                    tmp = track[1]->pLP[i];
                    tmp += knob_accel(val);
                    //  a zero loop time will freeze the dsp...
                    if (tmp < 1) tmp = 256;
                    if (tmp > AUX_SIZE_1) tmp = AUX_SIZE_1;
                    track[1]->pLP[i] = tmp;
                    ctl_param_change(i - 16, eParamLoop1, tmp);
                    render_track1(14);
                }
                
                else ;
            }
            break;
            
        case 3: //mode parameter 3
            check_touch(kEventEncoder2);
            if (touchedThis) {
                
                //off
                if (track[1]->m[i] == 0) ;
                
                //HOLD:
                else if (track[1]->m[i] == 1) ;
                
                //TRIG
                else if (track[1]->m[i] == 2) ;
                
                //GATE
                else if (track[1]->m[i] == 3) ;
                
                //NOISE
                else if (track[1]->m[i] == 4) ;
                
                //one:
                else if (track[1]->m[i] == 5) ;
                
                //loop
                else if (track[1]->m[i] == 6) ;
                
                //wav
                else if (track[1]->m[i] == 7) ;
                
                //noise
                else if (track[1]->m[i] == 8) ;
                
                //rec
                else if (track[1]->m[i] == 9) ;
                
                //TRIGrec
                else if (track[1]->m[i] == 10) ;
                
                //[aux master]
                else if (track[1]->m[i] == 11) ;
                
                //[insert mix]: mix
                else if (track[1]->m[i] == 12) {
                    tmp = track[1]->mix;
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[1]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix1, tmp);
                    render_track1(12);
                }
                
                //[delay]: mix
                else if (track[1]->m[i] == 13) {
                    tmp = track[1]->mix;
                    tmp += val * 4194304;
                    track[1]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix1, tmp);
                    render_track1(13);
                }
                
                //[diffuse]: mix
                else if (track[1]->m[i] == 14) {
                    tmp = track[1]->mix;
                    tmp += val * 4194304;
                    track[1]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix1, tmp);
                    render_track1(14);
                }

                else ;
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
                render_aux_param(1);
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
                /*
                if (val > 0)
                    bfin_set_trig();
                else if (val < 0)
                    bfin_set_reversetrig();
                */
            }
            break;
            
        case 1: //mode parameter 1
            check_touch(kEventEncoder1);
            if (touchedThis) {
                
                //off
                if (track[2]->m[i] == 0) ;
                
                //HOLD: cv output level
                else if (track[2]->m[i] == 1)
                {
                    tmp = track[2]->pF[i];
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[2]->pF[i] = tmp;
                    ctl_param_change(i - 16, eParamFrequency2, tmp);
                    render_track2(1);
                }
                
                //TRIG
                else if (track[2]->m[i] == 2) ;
                
                //GATE
                else if (track[2]->m[i] == 3) ;
                
                //NOISE
                else if (track[2]->m[i] == 4) ;
                
                //one: recording buffer position
                else if (track[2]->m[i] == 5)
                {
                    tmp = track[2]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[2]->offset) tmp = sample[2]->offset;
                    if (tmp > sample[3]->offset) tmp = sample[3]->offset;
                    track[2]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition2, tmp);
                    render_track2(5);
                }
                
                //loop: recording buffer position
                else if (track[2]->m[i] == 6)
                {
                    tmp = track[2]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[2]->offset) tmp = sample[2]->offset;
                    if (tmp > sample[3]->offset) tmp = sample[3]->offset;
                    track[2]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition2, tmp);
                    render_track2(6);
                }
                
                //wav: sample
                else if (track[2]->m[i] == 7)
                {
                    tmp = track[2]->pS[i];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[2]->pS[i] = tmp;
                    track[2]->pP[i] = sample[tmp]->offset;
                    track[2]->pLP[i] = sample[tmp]->size - 1;
                    ctl_param_change(i - 16, eParamPosition2, track[2]->pP[i]);
                    ctl_param_change(i - 16, eParamLoop2, track[2]->pLP[i]);
                    render_track2(7);
                }
                
                //noise
                else if (track[2]->m[i] == 8) ;
                
                //rec: input A
                else if (track[2]->m[i] == 9) {
                    tmp = track[2]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[2]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA2, tmp);
                    render_track2(9);
                }
            
                //TRIGrec: physical input A 
                else if (track[2]->m[i] == 10) {
                    tmp = track[2]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_PHYSICAL_INPUTS_1) tmp = N_PHYSICAL_INPUTS_1;
                    track[2]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA2, tmp);
                    render_track2(10);
                }
            
                //[aux master]
                else if (track[2]->m[i] == 11) ;
                
                //[insert mix]: input A
                else if (track[2]->m[i] == 12) {
                    tmp = track[2]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[2]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA2, tmp);
                    render_track2(12);
                }
                
                //[delay]: input A
                else if (track[2]->m[i] == 13) {
                    tmp = track[2]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[2]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA2, tmp);
                    render_track2(13);
                }
                
                //[diffuse]: input A
                else if (track[2]->m[i] == 14) {
                    tmp = track[2]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[2]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA2, tmp);
                    render_track2(14);
                }
                
                //<GLOBAL>: sq length
                else if (track[2]->m[i] == 15)
                {
                    tmp = length;
                    tmp += val;
                    if (tmp < 1 + 16) tmp = 1 + 16;
                    if (tmp > SQ_LEN) tmp = SQ_LEN;
                    length = tmp;
                    ctl_param_change(DUMMY, eParamSqLength, length - 16);
                    render_track2(15);
                }

                else ;
            }
            break;
            
        case 2: //mode parameter 2
            check_touch(kEventEncoder1);
            if (touchedThis) {
                
                //off
                if (track[2]->m[i] == 0) ;
                
                //HOLD:
                else if (track[2]->m[i] == 1) ;
                
                //TRIG
                else if (track[2]->m[i] == 2) ;
                
                //GATE
                else if (track[2]->m[i] == 3) ;
                
                //NOISE
                else if (track[2]->m[i] == 4) ;
                
                //one:
                else if (track[2]->m[i] == 5) ;
                
                //loop:
                else if (track[2]->m[i] == 6) ;
                
                //wav: sample start position
                else if (track[2]->m[i] == 7)
                {
                    tmp = track[2]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[track[2]->pS[i]]->offset) tmp = sample[track[2]->pS[i]]->offset;
                    if (tmp > sample[track[2]->pS[i] + 1]->offset) tmp = sample[track[2]->pS[i] + 1]->offset;
                    track[2]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition2, tmp);
                    render_track2(7);
                }
                
                //noise
                else if (track[2]->m[i] == 8) ;
                
                //rec
                else if (track[2]->m[i] == 9) ;
                
                //TRIGrec
                else if (track[2]->m[i] == 10) ;
                
                //[aux master]
                else if (track[2]->m[i] == 11) ;
                
                //[insert mix]: input B
                else if (track[2]->m[i] == 12) {
                    tmp = track[2]->inB;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[2]->inB = tmp;
                    ctl_param_change(DUMMY, eParamInputB2, tmp);
                    render_track2(12);
                }
                
                //[delay]: loop
                else if (track[2]->m[i] == 13) {
                    tmp = track[2]->pLP[i];
                    tmp += knob_accel(val);
                    //  a zero loop time will freeze the dsp...
                    if (tmp < 1) tmp = 256;
                    if (tmp > AUX_SIZE_1) tmp = AUX_SIZE_1;
                    track[2]->pLP[i] = tmp;
                    ctl_param_change(i - 16, eParamLoop2, tmp);
                    render_track2(13);
                }
                
                //[diffuse]: time
                else if (track[2]->m[i] == 14) {
                    tmp = track[2]->pLP[i];
                    tmp += knob_accel(val);
                    //  a zero loop time will freeze the dsp...
                    if (tmp < 1) tmp = 256;
                    if (tmp > AUX_SIZE_1) tmp = AUX_SIZE_1;
                    track[2]->pLP[i] = tmp;
                    ctl_param_change(i - 16, eParamLoop2, tmp);
                    render_track2(14);
                }

                else ;
            }
            break;
            
        case 3: //mode parameter 3
            check_touch(kEventEncoder1);
            if (touchedThis) {
                
                //off
                if (track[2]->m[i] == 0) ;
                
                //HOLD:
                else if (track[2]->m[i] == 1) ;
                
                //TRIG
                else if (track[2]->m[i] == 2) ;
                
                //GATE
                else if (track[2]->m[i] == 3) ;
                
                //NOISE
                else if (track[2]->m[i] == 4) ;
                
                //one:
                else if (track[2]->m[i] == 5) ;
                
                //loop
                else if (track[2]->m[i] == 6) ;
                
                //wav
                else if (track[2]->m[i] == 7) ;
                
                //noise
                else if (track[2]->m[i] == 8) ;
                
                //rec
                else if (track[2]->m[i] == 9) ;
                
                //TRIGrec
                else if (track[2]->m[i] == 10) ;
                
                //[aux master]
                else if (track[2]->m[i] == 11) ;
                
                //[insert mix]: mix
                else if (track[2]->m[i] == 12) {
                    tmp = track[2]->mix;
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[2]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix2, tmp);
                    render_track2(12);
                }
                
                //[delay]: mix
                else if (track[2]->m[i] == 13) {
                    tmp = track[2]->mix;
                    tmp += val * 4194304;
                    track[2]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix2, tmp);
                    render_track2(13);
                }
                
                //[diffuse]: mix
                else if (track[2]->m[i] == 14) {
                    tmp = track[2]->mix;
                    tmp += val * 4194304;
                    track[2]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix2, tmp);
                    render_track2(14);
                }

                else ;
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
                render_aux_param(2);
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
                
                render_track0(track[0]->m[i]);
                render_track1(track[1]->m[i]);
                render_track2(track[2]->m[i]);
                render_track3(track[3]->m[i]);

                print_fix16(renderEditPosition, (i + 1 - 16) * 0x00010000);
                
                render_counters_param();
            }
            break;

        case 1: //mode parameter 1
            check_touch(kEventEncoder0);
            if (touchedThis) {
                
                //off
                if (track[3]->m[i] == 0) ;
                
                //HOLD: cv output level
                else if (track[3]->m[i] == 1)
                {
                    tmp = track[3]->pF[i];
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[3]->pF[i] = tmp;
                    ctl_param_change(i - 16, eParamFrequency3, tmp);
                    render_track3(1);
                }
                
                //TRIG
                else if (track[3]->m[i] == 2) ;
                
                //GATE
                else if (track[3]->m[i] == 3) ;
                
                //NOISE
                else if (track[3]->m[i] == 4) ;
                
                //one: recording buffer position
                else if (track[3]->m[i] == 5)
                {
                    tmp = track[3]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[3]->offset) tmp = sample[3]->offset;
                    if (tmp > sample[4]->offset) tmp = sample[4]->offset;
                    track[3]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition3, tmp);
                    render_track3(5);
                }
                
                //loop: recording buffer position
                else if (track[3]->m[i] == 6)
                {
                    tmp = track[3]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[3]->offset) tmp = sample[3]->offset;
                    if (tmp > sample[4]->offset) tmp = sample[4]->offset;
                    track[3]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition3, tmp);
                    render_track3(6);
                }
                
                //wav: sample
                else if (track[3]->m[i] == 7)
                {
                    tmp = track[3]->pS[i];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[3]->pS[i] = tmp;
                    track[3]->pP[i] = sample[tmp]->offset;
                    track[3]->pLP[i] = sample[tmp]->size - 1;
                    ctl_param_change(i - 16, eParamPosition3, track[3]->pP[i]);
                    ctl_param_change(i - 16, eParamLoop3, track[3]->pLP[i]);
                    render_track3(7);
                }
                
                //noise
                else if (track[3]->m[i] == 8) ;
                
                //rec: input A
                else if (track[3]->m[i] == 9) {
                    tmp = track[3]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[3]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA3, tmp);
                    render_track3(9);
                }
                
                //TRIGrec: physical input A
                else if (track[3]->m[i] == 10) {
                    tmp = track[3]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_PHYSICAL_INPUTS_1) tmp = N_PHYSICAL_INPUTS_1;
                    track[3]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA3, tmp);
                    render_track3(10);
                }
            
                //[aux master]
                else if (track[3]->m[i] == 11) ;
                
                //[insert mix]: input A
                else if (track[3]->m[i] == 12) {
                    tmp = track[3]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[3]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA3, tmp);
                    render_track3(12);
                }
                
                //[delay]: input A
                else if (track[3]->m[i] == 13) {
                    tmp = track[3]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[3]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA3, tmp);
                    render_track3(13);
                }
                
                //[diffuse]: input A
                else if (track[3]->m[i] == 14) {
                    tmp = track[3]->inA;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[3]->inA = tmp;
                    ctl_param_change(DUMMY, eParamInputA3, tmp);
                    render_track3(14);
                }
                
                //<GLOBAL>: sq length
                else if (track[3]->m[i] == 15)
                {
                    tmp = length;
                    tmp += val;
                    if (tmp < 1 + 16) tmp = 1 + 16;
                    if (tmp > SQ_LEN) tmp = SQ_LEN;
                    length = tmp;
                    ctl_param_change(DUMMY, eParamSqLength, length - 16);
                    render_track3(15);
                }

                else ;
            }
            break;
            
        case 2: //mode parameter 2
            check_touch(kEventEncoder0);
            if (touchedThis) {
                
                //off
                if (track[3]->m[i] == 0) ;
                
                //HOLD:
                else if (track[3]->m[i] == 1) ;
                
                //TRIG
                else if (track[3]->m[i] == 2) ;
                
                //GATE
                else if (track[3]->m[i] == 3) ;
                
                //NOISE
                else if (track[3]->m[i] == 4) ;
                
                //one:
                else if (track[3]->m[i] == 5) ;
                
                //loop:
                else if (track[3]->m[i] == 6) ;
                
                //wav: sample buffer start position
                else if (track[3]->m[i] == 7)
                {
                    tmp = track[3]->pP[i];
                    tmp += knob_accel(val);
                    if (tmp < sample[track[3]->pS[i]]->offset) tmp = sample[track[3]->pS[i]]->offset;
                    if (tmp > sample[track[3]->pS[i] + 1]->offset) tmp = sample[track[3]->pS[i] + 1]->offset;
                    track[3]->pP[i] = tmp;
                    ctl_param_change(i - 16, eParamPosition3, tmp);
                    render_track3(7);
                }
                
                //noise
                else if (track[3]->m[i] == 8) ;
                
                //rec
                else if (track[3]->m[i] == 9) ;
            
                //TRIGrec
                else if (track[3]->m[i] == 10) ;
                
                //[aux master]
                else if (track[3]->m[i] == 11) ;
                
                //[insert mix]: input B
                else if (track[3]->m[i] == 12) {
                    tmp = track[3]->inB;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                    track[3]->inB = tmp;
                    ctl_param_change(DUMMY, eParamInputB3, tmp);
                    render_track3(12);
                }
                
                //[delay]: loop
                else if (track[3]->m[i] == 13) {
                    tmp = track[3]->pLP[i];
                    tmp += knob_accel(val);
                    //  a zero loop time will freeze the dsp...
                    if (tmp < 1) tmp = 256;
                    if (tmp > AUX_SIZE_1) tmp = AUX_SIZE_1;
                    track[3]->pLP[i] = tmp;
                    ctl_param_change(i - 16, eParamLoop3, tmp);
                    render_track3(13);
                }
                
                //[diffuse]: time
                else if (track[3]->m[i] == 14) {
                    tmp = track[3]->pLP[i];
                    tmp += knob_accel(val);
                    //  a zero loop time will freeze the dsp...
                    if (tmp < 1) tmp = 256;
                    if (tmp > AUX_SIZE_1) tmp = AUX_SIZE_1;
                    track[3]->pLP[i] = tmp;
                    ctl_param_change(i - 16, eParamLoop3, tmp);
                    render_track3(14);
                }
                
                else ;
            }
            break;
            
        case 3: //mode parameter 3
            check_touch(kEventEncoder0);
            if (touchedThis) {
                
                //off
                if (track[3]->m[i] == 0) ;
                
                //HOLD:
                else if (track[3]->m[i] == 1) ;
                
                //TRIG
                else if (track[3]->m[i] == 2) ;
                
                //GATE
                else if (track[3]->m[i] == 3) ;
                
                //NOISE
                else if (track[3]->m[i] == 4) ;
                
                //one:
                else if (track[3]->m[i] == 5) ;
                
                //loop
                else if (track[3]->m[i] == 6) ;
                
                //wav
                else if (track[3]->m[i] == 7) ;
                
                //noise
                else if (track[3]->m[i] == 8) ;
                
                //rec
                else if (track[3]->m[i] == 9) ;
                
                //TRIGrec
                else if (track[3]->m[i] == 10) ;
                
                //[aux master]
                else if (track[3]->m[i] == 11) ;
                
                //[insert mix]: mix
                else if (track[3]->m[i] == 12) {
                    tmp = track[3]->mix;
                    tmp += knob_accel(val);
                    if (tmp < 0) tmp = 0;
                    track[3]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix3, tmp);
                    render_track3(12);
                }
                
                //[delay]: mix
                else if (track[3]->m[i] == 13) {
                    tmp = track[3]->mix;
                    tmp += val * 4194304;
                    track[3]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix3, tmp);
                    render_track3(13);
                }
                
                //[diffuse]: mix
                else if (track[3]->m[i] == 14) {
                    tmp = track[3]->mix;
                    tmp += val * 4194304;
                    track[3]->mix = tmp;
                    ctl_param_change(DUMMY, eParamMix3, tmp);
                    render_track3(14);
                }
                
                else ;
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
                render_aux_param(3);
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

/*
//scrub
s32 end;
tmp = track[0]->pP[i];
end = tmp + SCRUB_SIZE;
tmp += knob_accel(val);
end += knob_accel(val);
if (tmp < 0) { tmp = 0; end = SCRUB_SIZE; }
track[0]->pP[i] = tmp;
ctl_param_change(i, eParamPosition0, tmp);
ctl_param_change(i, eParamTime0, end);
//  ctl_param_change(eParamTrig0, 1);
//  send bfin_trig_message...
print_fix16(renderPosition0, tmp);
render_track0(7);
*/

/*
 //set buffer length
 //                    frames = measure * tempo * FRAMES;
 //                    ctl_param_change(i, eParamBufferLength0, frames);
 //                    ctl_param_change(i, eParamBufferLength1, frames);
 //                    ctl_param_change(i, eParamBufferLength2, frames);
 //                    ctl_param_change(i, eParamBufferLength3, frames);
 //  set sequencer length
 
 //if step length is aligned to measure, recalculate step length and update bfin
 for (i=0; i<SQ_LEN; i++)
 {
 if (n_scale[i] >= 1)
 {
 n_scale[i] = measure;
 frames = measure * tempo * FRAMES;
 ctl_param_change(i, eParamFrames, frames);
 }
 }
 */

void select_param(void) {
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
