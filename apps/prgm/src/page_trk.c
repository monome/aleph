//aleph-prgm-avr32

#include "page_trk.h"

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

static void set_mode(u8 track, u8 mode);
static s32 knob_accel(s32 inc);


//handler variables
static etype touched = kNumEventTypes;
static u8 touchedThis = 0;
static u8 state_sw;

static const int eParamTrigId[] =
{
    eParamTrig0,
    eParamTrig1,
    eParamTrig2,
    eParamTrig3,
    eParamTrig4,
    eParamTrig5,
    eParamTrig6,
    eParamTrig7,
};

static const int eParamFlagId[] =
{
    eParamFlag0,
    eParamFlag1,
    eParamFlag2,
    eParamFlag3,
    eParamFlag4,
    eParamFlag5,
    eParamFlag6,
    eParamFlag7,
};

static const int eParamCurveId[] =
{
    eParamCurve0,
    eParamCurve1,
    eParamCurve2,
    eParamCurve3,
    eParamCurve4,
    eParamCurve5,
    eParamCurve6,
    eParamCurve7,
};

static const int eParamOffsetAId[] =
{
    eParamOffsetA0,
    eParamOffsetA1,
    eParamOffsetA2,
    eParamOffsetA3,
    eParamOffsetA4,
    eParamOffsetA5,
    eParamOffsetA6,
    eParamOffsetA7,
};

static const int eParamOffsetBId[] =
{
    eParamOffsetB0,
    eParamOffsetB1,
    eParamOffsetB2,
    eParamOffsetB3,
    eParamOffsetB4,
    eParamOffsetB5,
    eParamOffsetB6,
    eParamOffsetB7,
};

static const int eParamSampleOffsetAId[] =
{
    eParamSampleOffsetA0,
    eParamSampleOffsetA1,
    eParamSampleOffsetA2,
    eParamSampleOffsetA3,
    eParamSampleOffsetA4,
    eParamSampleOffsetA5,
    eParamSampleOffsetA6,
    eParamSampleOffsetA7,
};

static const int eParamSampleOffsetBId[] =
{
    eParamSampleOffsetB0,
    eParamSampleOffsetB1,
    eParamSampleOffsetB2,
    eParamSampleOffsetB3,
    eParamSampleOffsetB4,
    eParamSampleOffsetB5,
    eParamSampleOffsetB6,
    eParamSampleOffsetB7,
};

static const int eParamUnsignedId[] =
{
    eParamUnsigned0,
    eParamUnsigned1,
    eParamUnsigned2,
    eParamUnsigned3,
    eParamUnsigned4,
    eParamUnsigned5,
    eParamUnsigned6,
    eParamUnsigned7,
};

static const int eParamSignedId[] =
{
    eParamSigned0,
    eParamSigned1,
    eParamSigned2,
    eParamSigned3,
    eParamSigned4,
    eParamSigned5,
    eParamSigned6,
    eParamSigned7,
};

static const int eParamInputId[] =
{
    eParamInput0,
    eParamInput1,
    eParamInput2,
    eParamInput3,
    eParamInput4,
    eParamInput5,
    eParamInput6,
    eParamInput7,
};

static const int eParamInputLevelId[] =
{
    eParamInputLevel0,
    eParamInputLevel1,
    eParamInputLevel2,
    eParamInputLevel3,
    eParamInputLevel4,
    eParamInputLevel5,
    eParamInputLevel6,
    eParamInputLevel7,
};

static const int eParamMixPanLId[] =
{
    eParamMixPanL0,
    eParamMixPanL1,
    eParamMixPanL2,
    eParamMixPanL3,
    eParamMixPanL4,
    eParamMixPanL5,
    eParamMixPanL6,
    eParamMixPanL7,
};

static const int eParamMixPanRId[] =
{
    eParamMixPanR0,
    eParamMixPanR1,
    eParamMixPanR2,
    eParamMixPanR3,
    eParamMixPanR4,
    eParamMixPanR5,
    eParamMixPanR6,
    eParamMixPanR7,
};

static const int eParamMixLevelId[] =
{
    eParamMixLevel0,
    eParamMixLevel1,
    eParamMixLevel2,
    eParamMixLevel3,
    eParamMixLevel4,
    eParamMixLevel5,
    eParamMixLevel6,
    eParamMixLevel7,
};

static const int eParamAux1LevelId[] =
{
    eParamAux1Level0,
    eParamAux1Level1,
    eParamAux1Level2,
    eParamAux1Level3,
    eParamAux1Level4,
    eParamAux1Level5,
    eParamAux1Level6,
    eParamAux1Level7,
};

static const int eParamAux2LevelId[] =
{
    eParamAux2Level0,
    eParamAux2Level1,
    eParamAux2Level2,
    eParamAux2Level3,
    eParamAux2Level4,
    eParamAux2Level5,
    eParamAux2Level6,
    eParamAux2Level7,
};

static const int eParamLengthId[] =
{
    eParamLength0,
    eParamLength1,
    eParamLength2,
    eParamLength3,
    eParamLength4,
    eParamLength5,
    eParamLength6,
    eParamLength7,
};

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
    u8 t = trkpage;
    
    handle_sw(4, data > 0);
    
    if(state_sw == 4)
    {
        if (!track[t]->mutemix)
        {
            track[t]->mutemix = 1;
            ctl_param_change(DUMMY, eParamMixLevelId[t], 0);
            render_track(t);
        }
        
        else
        {
            track[t]->mutemix = 0;
            ctl_param_change(DUMMY, eParamMixLevelId[t], track[t]->mix);
            render_track(t);
        }
    }
}

//flip page to sequencer
void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        set_page(ePageSeq);
        render_seq();
    }
    else ;;
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    u8 t = trkpage;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[t]->pan;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[t]->pan = tmp;
                ctl_param_change(DUMMY, eParamMixPanLId[t], sub_fr1x32(FR32_MAX, tmp));
                ctl_param_change(DUMMY, eParamMixPanRId[t], tmp);
                render_track(t);
            }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[t]->input;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_INPUTS_1) tmp = N_INPUTS_1;
                track[t]->input = tmp;
                ctl_param_change(DUMMY, eParamInputId[t], tmp);
                render_track(t);
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
    u8 t = trkpage;
    switch (state_sw) {
        case 0: //mix level
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[t]->mix;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[t]->mix = tmp;
                if (!track[t]->mutemix)
                {
                    ctl_param_change(DUMMY, eParamMixLevelId[t], tmp);
                }
                render_track(t);
            }
            break;
            
        case 1: //input level
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[t]->inL;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[t]->inL = tmp;
                ctl_param_change(DUMMY, eParamInputLevelId[t], tmp);
                render_track(t);
            }
            break;
            
        case 2: //aux 1 level
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[t]->aux1;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[t]->aux1 = tmp;
                ctl_param_change(DUMMY, eParamAux1LevelId[t], tmp);
                render_track(t);
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
    s32 tmp;
    u8 t = trkpage;
    u32 i = editpos;
    switch (state_sw) {
        case 0: //mode parameter 1
            check_touch(kEventEncoder1);
            if (touchedThis) {
                if (track[t]->m == 0) ;
                else if (track[t]->m == 1) ;
                else if (track[t]->m == 2) ;
                else if (track[t]->m == 3) ;
                else if (track[t]->m == 4)
                {
                    tmp = track[t]->uP;
                    tmp += knob_accel(val);
                    if (tmp < sample[t]->offset + LOOP_MIN) tmp = sample[t]->offset + LOOP_MIN;
                    if (tmp > sample[t+1]->offset) tmp = sample[t+1]->offset;
                    track[t]->uP = tmp;
                    ctl_param_change(DUMMY, eParamUnsignedId[t], tmp);
                    render_track(t);
                }
                else if (track[t]->m == 5) ;
                else ;
            }
            break;
            
        case 1: //mode parameter 2
            check_touch(kEventEncoder1);
            if (touchedThis) {
                if (track[t]->m == 0) ;
                else if (track[t]->m == 1) ;
                else if (track[t]->m == 2) ;
                else if (track[t]->m == 3) ;
                else if (track[t]->m == 4)
                {
                    tmp = track[t]->sP;
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[t]->sP = tmp;
                    ctl_param_change(DUMMY, eParamSignedId[t], tmp);
                    render_track(t);
                }
                else if (track[t]->m == 5) ;
                else ;
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
            
        case 4: //mode parameter 1
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
    u8 t = trkpage;
    switch (state_sw) {
        case 0: //track
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = t;
                tmp += val;
                if (tmp < 0)
                {
                    tmp = 0;
                    trkpage = tmp;
                    render_track(tmp);
                }
                if (tmp < N_TRACKS)
                {
                    trkpage = tmp;
                    render_track(tmp);
                }
                else
                {
                    trkpage = N_TRACKS_1;
                    set_page(ePageMix);
                    render_mix();
                    handle_switch_3(0);
                    break;
                }
            }
            break;
            
        case 1: //track mode
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[t]->m;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_MODES_1) tmp = N_MODES_1;
                track[t]->m = tmp;
                set_mode(t, tmp);
                render_track(t);
            }
            break;
            
        case 2: //aux 2 level
            check_touch(kEventEncoder0);
            if (touchedThis) {                
                tmp = track[t]->aux2;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[t]->aux2 = tmp;
                ctl_param_change(DUMMY, eParamAux2LevelId[t], tmp);
                render_track(t);
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

void set_mode(u8 n, u8 m) {
    prgmTrack *t = track[n];
    
    //  unmute track
    if (t->mutetrk) t->mutetrk = 0;
    
    //  set mode
    //  off
    if(m == 0)
    {
        t->m = 0;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 0);
    }
    
    //  trig
    else if(m == 1)
    {
        t->m = 1;
        t->s[editpos] = 8;
        ctl_param_change(editpos, eParamSampleOffsetBId[n], sample[9]->offset);
        ctl_param_change(editpos, eParamSampleOffsetAId[n], sample[8]->offset);
        ctl_param_change(DUMMY, eParamFlagId[n], 1);
        ctl_param_change(DUMMY, eParamCurveId[n], 1);
        ctl_param_change(DUMMY, eParamTrigId[n], DUMMY);
    }
    
    //  loop
    else if(m == 2)
    {
        t->m = 2;
        t->s[editpos] = 8;
        ctl_param_change(editpos, eParamSampleOffsetBId[n], sample[9]->offset);
        ctl_param_change(editpos, eParamSampleOffsetAId[n], sample[8]->offset);
        ctl_param_change(DUMMY, eParamFlagId[n], 1);
        ctl_param_change(DUMMY, eParamCurveId[n], 2);
        ctl_param_change(DUMMY, eParamTrigId[n], DUMMY);
    }
    
    //  [amp]
    else if(m == 3)
    {
        t->m = 3;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 0);
    }

    //  [dly]
    else if(m == 4)
    {
        t->m = 4;
        ctl_param_change(DUMMY, eParamOffsetBId[n], sample[n+1]->offset);
        ctl_param_change(DUMMY, eParamOffsetAId[n], sample[n]->offset);
        ctl_param_change(DUMMY, eParamFlagId[n], 3);
        ctl_param_change(DUMMY, eParamCurveId[n], 4);
        ctl_param_change(DUMMY, eParamTrigId[n], DUMMY);
    }
        
    //  [thru]
    else if(m == 5)
    {
        t->m = 5;
        ctl_param_change(DUMMY, eParamFlagId[n], 4);
        ctl_param_change(DUMMY, eParamCurveId[n], 5);
    }
        
    else ;
}


void select_trk(void) {
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
