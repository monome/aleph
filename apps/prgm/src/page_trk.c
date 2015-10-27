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
static s32 tape_accel(s32 inc);


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

static const int eParamMuteFlagId[] =
{
    eParamMuteFlag0,
    eParamMuteFlag1,
    eParamMuteFlag2,
    eParamMuteFlag3,
    eParamMuteFlag4,
    eParamMuteFlag5,
    eParamMuteFlag6,
    eParamMuteFlag7,
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

static const int eParamAuxLevelId[] =
{
    eParamAuxLevel0,
    eParamAuxLevel1,
    eParamAuxLevel2,
    eParamAuxLevel3,
    eParamAuxLevel4,
    eParamAuxLevel5,
    eParamAuxLevel6,
    eParamAuxLevel7,
};

static const int eParamMotorId[] =
{
    eParamMotor0,
    eParamMotor1,
    eParamMotor2,
    eParamMotor3,
    eParamMotor4,
    eParamMotor5,
    eParamMotor6,
    eParamMotor7,
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
    if(incAbs < 8) {
        return inc << 2;
    }
    if(incAbs < 32) {
        return inc << 4;
    }
    return inc << 6;
}

s32 tape_accel(s32 inc) {
    s32 incAbs = inc < 0 ? inc * -1 : inc;
    if(incAbs == 1) {
        return inc << 1;
    }
    if(incAbs < 4) {
        return inc << 3;
    }
    if(incAbs < 16) {
        return inc << 5;
    }
    if(incAbs < 36) {
        return inc << 7;
    }
    if(incAbs < 64) {
        return inc << 11;
    }
    return inc << 13;
}

void handle_switch_0(s32 data) {
    handle_sw(1, data > 0);
}

void handle_switch_1(s32 data) {
    handle_sw(2, data > 0);
}

void handle_switch_2(s32 data) {
    u8 t = trkpage;
    
    handle_sw(3, data > 0);
    
    //  deck TEST SEND SELECTED MESSAGE
    if(state_sw == 3 && track[t]->m == 2)
    {
        //send message...
    }
    
    //  tape
    if(state_sw == 3 && track[t]->m == 3)
    {
        if (!track[t]->motor)
        {
            //  start motor
            track[t]->motor = 1;
            ctl_param_change(DUMMY, eParamMotorId[t], 1);
        }
        else
        {
            //  stop motor
            track[t]->motor = 0;
            ctl_param_change(DUMMY, eParamMotorId[t], 0);
        }
    }
    
    //  amp
    if(state_sw == 3 && track[t]->m == 4)
    {
        ctl_param_change(DUMMY, eParamTrigId[t], DUMMY);
    }
}

void handle_switch_3(s32 data) {
    u8 t = trkpage;
    
    handle_sw(4, data > 0);
    
    if (state_sw == 4)
    {
        u8 i;

        if (track[t]->mutemix == MIX)
        {
            //  mute prepare
            prepare = 1;
            track[t]->mutemix = PREPARE;
        }
        else if (track[t]->mutemix == MUTE)
        {
            //  mute prepare
            prepare = 1;
            track[t]->mutemix = PREPAREMUTED;
        }
        else
        {
            //  clear mute prepare
            prepare = 0;
            if (track[t]->mutemix == PREPARE) track[t]->mutemix = MIX;
            if (track[t]->mutemix == PREPAREMUTED) track[t]->mutemix = MUTE;
            for (i=0; i<N_TRACKS; i++)
            {
                if (track[i]->mutemix == PREPARE || track[i]->mutemix == PREPAREMUTED) prepare = 1;
            }
        }
        render_track(t);
    }
}

//flip page to sequencer
void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        //  move editpos in case outside range
//        if (editpos > track[trkpage]->len) editpos = track[trkpage]->len;
        set_page(ePageSeq);
        render_seq();
    }
    else ;;
}

void handle_encoder_0(s32 val) {
    s32 tmp, aux;
    u8 t = trkpage;
    switch (state_sw) {
        case 0: //aux send level
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[t]->aux;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[t]->aux = tmp;
                //  aux pre fader
                if (track[t]->auxsw == 0)
                {
                    ctl_param_change(DUMMY, eParamAuxLevelId[t], track[t]->aux);
                }
                //  aux mute
                if (track[t]->auxsw == 1)
                {
                    ;
                }
                //  aux post fader
                if (track[t]->auxsw == 2)
                {
                    aux = mult_fr1x32x32(track[t]->aux, track[t]->mix);
                    ctl_param_change(DUMMY, eParamAuxLevelId[t], aux);
                }
                render_track(t);
            }
            break;
            
        case 1: //aux pre|mute|post
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = track[t]->auxsw;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 2) tmp = 2;
                track[t]->auxsw = tmp;
                //  aux pre fader
                if (tmp == 0)
                {
                    ctl_param_change(DUMMY, eParamAuxLevelId[t], track[t]->aux);
                }
                //  aux mute
                if (tmp == 1)
                {
                    ctl_param_change(DUMMY, eParamAuxLevelId[t], 0);
                }
                //  aux post fader
                if (tmp == 2)
                {
                    aux = mult_fr1x32x32(track[t]->aux, track[t]->mix);
                    ctl_param_change(DUMMY, eParamAuxLevelId[t], aux);
                }
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
    s32 tmp, aux;
    u8 t = trkpage;
    switch (state_sw) {
        case 0: //mix level
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[t]->mix;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                track[t]->mix = tmp;
                if (solo)
                {
                    if (track[t]->mutemix == SOLO) ctl_param_change(DUMMY, eParamMixLevelId[t], tmp);
                }
                if (!solo)
                {
                    if (track[t]->mutemix != MUTE) ctl_param_change(DUMMY, eParamMixLevelId[t], tmp);
                }
                //  aux post fader
                if (track[t]->auxsw == 2)
                {
                    aux = mult_fr1x32x32(track[t]->aux, track[t]->mix);
                    ctl_param_change(DUMMY, eParamAuxLevelId[t], aux);
                }
                render_track(t);
            }
            break;
            
        case 1: //mix|group
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = track[t]->mixsw;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > 5) tmp = 5;
                track[t]->mixsw = tmp;
                ctl_param_change(t, eParamRouteMix, tmp);
                render_track(t);
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
    s32 tmp;
    u8 t = trkpage;
    switch (state_sw) {
        case 0: //mode parameter 1
            check_touch(kEventEncoder1);
            if (touchedThis) {
                if (track[t]->m == 0) ;
                else if (track[t]->m == 1) ;
                //  deck TEST MESSAGE SELECT
                else if (track[t]->m == 2)
                {
                    tmp = track[t]->motor;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > 1) tmp = 1;
                    track[t]->motor = tmp;
                    render_track(t);
                }            
                //  tape
                else if (track[t]->m == 3)
                {
                    tmp = track[t]->sP;
                    tmp += tape_accel(val);
                    //  scrub
                    if (!track[t]->motor)
                    {
                        if (tmp < 0) //if (val < 0)
                        {
                            if (track[t]->motordir)
                            {
                                //  reverse playback
                                track[t]->motordir = 0;
                                ctl_param_change(DUMMY, eParamCurveId[t], 7);
                                ctl_param_change(DUMMY, eParamTrigId[t], DUMMY);
//                                ctl_param_change(DUMMY, eParamCurveId[t], 9);
                            }
                        }
                        if (tmp > 0) //if (val > 0)
                        {
                            if (!track[t]->motordir)
                            {
                                //  forward playback
                                track[t]->motordir = 1;
                                ctl_param_change(DUMMY, eParamCurveId[t], 6);
                                ctl_param_change(DUMMY, eParamTrigId[t], DUMMY);
//                                ctl_param_change(DUMMY, eParamCurveId[t], 8);
                            }
                        }
/*
                        if (tmp < 0 && !track[t]->motordir)
                        {
                            ctl_param_change(DUMMY, eParamSignedId[t], tmp);
//                            ctl_param_change(DUMMY, eParamSignedId[t], 0);
                        }
                        if (tmp < 0 && track[t]->motordir)
                        {
                            ctl_param_change(DUMMY, eParamSignedId[t], -tmp);
//                            ctl_param_change(DUMMY, eParamSignedId[t], 0);
                        }
                        if (tmp > 0 && !track[t]->motordir)
                        {
                            ctl_param_change(DUMMY, eParamSignedId[t], -tmp);
//                            ctl_param_change(DUMMY, eParamSignedId[t], 0);
                        }
                        if (tmp > 0 && track[t]->motordir)
                        {
                            ctl_param_change(DUMMY, eParamSignedId[t], tmp);
//                            ctl_param_change(DUMMY, eParamSignedId[t], 0);
                        }
*/
                    }
                    //  playback
                    if (track[t]->motor)
                    {
                        if (tmp < 0)
                        {
                            if (track[t]->motordir)
                            {
                                //  reverse playback
                                track[t]->motordir = 0;
                                ctl_param_change(DUMMY, eParamCurveId[t], 7);
                                ctl_param_change(DUMMY, eParamTrigId[t], DUMMY);
                            }
                        }
                        if (tmp > 0)
                        {
                            if (!track[t]->motordir)
                            {
                                //  forward playback
                                track[t]->motordir = 1;
                                ctl_param_change(DUMMY, eParamCurveId[t], 6);
                                ctl_param_change(DUMMY, eParamTrigId[t], DUMMY);
                            }
                        }
                    }
                    ctl_param_change(DUMMY, eParamSignedId[t], tmp);
                    track[t]->sP = tmp;
                    render_track(t);
                }
                //  [amp]
                else if (track[t]->m == 4)
                {
                    tmp = track[t]->env;
                    tmp += val;
                    if (tmp < 10) tmp = 10;
                    if (tmp > 14) tmp = 14;
                    track[t]->env = tmp;
                    ctl_param_change(DUMMY, eParamCurveId[t], tmp);
                    render_track(t);                    
                }
                //  [dly]
                else if (track[t]->m == 5)
                {
                    tmp = track[t]->uP;
                    tmp += knob_accel(val);
                    if (tmp < sample[t]->offset + LOOP_MIN) tmp = sample[t]->offset + LOOP_MIN;
                    if (tmp > sample[t+1]->offset) tmp = sample[t+1]->offset;
                    track[t]->uP = tmp;
                    ctl_param_change(DUMMY, eParamUnsignedId[t], tmp);
                    render_track(t);
                }
                //  [input]
                else if (track[t]->m == 6)
                {
                    tmp = track[t]->inL;
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[t]->inL = tmp;
                    ctl_param_change(DUMMY, eParamInputLevelId[t], tmp);
                    render_track(t);
                }
                //  CLK
                else if (track[t]->m == 7)
                {
                    tmp = master->bpm;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > 12) tmp = 12;
                    master->bpm = tmp;
                    if (tmp == 0) ctl_param_change(DUMMY, eParamBPM, 0x1F40); //90
                    else if (tmp == 1) ctl_param_change(DUMMY, eParamBPM, 0x1770); //120
                    else if (tmp == 2) ctl_param_change(DUMMY, eParamBPM, 0x12C0); //150
                    else if (tmp == 3) ctl_param_change(DUMMY, eParamBPM, 0xFA0); //180
                    else if (tmp == 4) ctl_param_change(DUMMY, eParamBPM, 0xBB8); //240
                    else if (tmp == 5) ctl_param_change(DUMMY, eParamBPM, 0x5DC); //480
                    else if (tmp == 6) ctl_param_change(DUMMY, eParamBPM, 0x2EE); //960
                    else if (tmp == 7) ctl_param_change(DUMMY, eParamBPM, 0x177); //1920
                    else if (tmp == 8) ctl_param_change(DUMMY, eParamBPM, 0xC8); //3600
                    else if (tmp == 9) ctl_param_change(DUMMY, eParamBPM, 0x64); //7200
                    else if (tmp == 10) ctl_param_change(DUMMY, eParamBPM, 0x32); //14400
                    else if (tmp == 11) ctl_param_change(DUMMY, eParamBPM, 0x19); //28800
                    else if (tmp == 12) ctl_param_change(DUMMY, eParamBPM, 0xA); //72000
                    else ;
                    render_track(t);
                }
                else ;
            }
            break;
            
        case 1: //mode parameter 2
            check_touch(kEventEncoder1);
            if (touchedThis) {
                if (track[t]->m == 0) ;
                else if (track[t]->m == 1) ;
                else if (track[t]->m == 2) ;
                //  tape sample
                else if (track[t]->m == 3)
                {
                    tmp = track[t]->s[DUMMY];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[t]->s[DUMMY] = tmp;
                    track[t]->sP = ((sample[tmp + 1]->offset + (sample[tmp]->loop_cut)) - (sample[tmp]->offset + sample[tmp]->offset_cut));
                    ctl_param_change(DUMMY, eParamOffsetBId[t], sample[tmp + 1]->offset + (sample[tmp]->loop_cut));
                    ctl_param_change(DUMMY, eParamOffsetAId[t], sample[tmp]->offset + sample[tmp]->offset_cut);
                    ctl_param_change(DUMMY, eParamSignedId[t], track[t]->sP);
                    ctl_param_change(DUMMY, eParamTrigId[t], DUMMY);
                    render_track(t);
                    break;
                }
                else if (track[t]->m == 4) ;
                //  [dly]
                else if (track[t]->m == 5)
                {
                    tmp = track[t]->sP;
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[t]->sP = tmp;
                    ctl_param_change(DUMMY, eParamSignedId[t], tmp);
                    render_track(t);
                }
                else if (track[t]->m == 6) ;
                else ;
            }
            break;
            
        case 2:
            check_touch(kEventEncoder1);
            if (touchedThis) {
                if (track[t]->m == 0) ;
                //  tape mode
                else if (track[t]->m == 3)
                {
                    tmp = track[t]->cue;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > 1) tmp = 1;
                    track[t]->cue = tmp;
                    //  cue on
                    if (tmp)
                    {
                        ctl_param_change(DUMMY, eParamFlagId[t], 6);
                    }
                    //  cue off
                    else
                    {
                        ctl_param_change(DUMMY, eParamFlagId[t], 5);
                    }
                    render_track(t);
                    break;
                }
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
                    handle_switch_0(0);
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
            
        case 2: //track input
            check_touch(kEventEncoder0);
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
    if (t->mutetrk == MUTE) t->mutetrk = TRACK;
    
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
        //  default playback: trig
        t->env = 1;
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
        ctl_param_change(DUMMY, eParamFlagId[n], 2);
        //  default interpolation: linear
        t->env = 3;
        ctl_param_change(DUMMY, eParamCurveId[n], 3);
        ctl_param_change(DUMMY, eParamTrigId[n], DUMMY);
    }
    
    //  tape
    else if(m == 3)
    {
        t->m = 3;
        t->s[editpos] = 8;
        ctl_param_change(DUMMY, eParamOffsetBId[n], sample[9]->offset);
        ctl_param_change(DUMMY, eParamOffsetAId[n], sample[8]->offset);
        //  default: motor off, tape speed 0, forward playback, cue off
        t->motor = 0;
        t->sP = 0;
        t->env = 6;
        t->cue = 0;
        ctl_param_change(DUMMY, eParamMotorId[n], 0);
        ctl_param_change(DUMMY, eParamSignedId[n], 0);
        ctl_param_change(DUMMY, eParamFlagId[n], 5);
        ctl_param_change(DUMMY, eParamCurveId[n], 6);
        ctl_param_change(DUMMY, eParamTrigId[n], DUMMY);
    }
    
    //  [amp]
    else if(m == 4)
    {
        t->m = 4;
        ctl_param_change(DUMMY, eParamFlagId[n], 2);
        //  default envelope: lindec
        t->env = 10;
        ctl_param_change(DUMMY, eParamCurveId[n], 10);
    }

    //  [dly]
    else if(m == 5)
    {
        t->m = 5;
        ctl_param_change(DUMMY, eParamOffsetBId[n], sample[n+1]->offset);
        ctl_param_change(DUMMY, eParamOffsetAId[n], sample[n]->offset);
        ctl_param_change(DUMMY, eParamFlagId[n], 3);
        ctl_param_change(DUMMY, eParamCurveId[n], 15);
        ctl_param_change(DUMMY, eParamTrigId[n], DUMMY);
    }
        
    //  [input]
    else if(m == 6)
    {
        t->m = 6;
        ctl_param_change(DUMMY, eParamFlagId[n], 4);
        ctl_param_change(DUMMY, eParamCurveId[n], 16);
    }
    
    //  CLK
    else if(m == 7)
    {
        t->m = 7;
        ctl_param_change(DUMMY, eParamFlagId[n], 10);
        ctl_param_change(DUMMY, eParamCurveId[n], 0);
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
