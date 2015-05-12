//  page_seq.c
//  aleph
//  Created by staffanjansson on 4/19/15.

#include "page_seq.h"

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

static void track_unmute(u8 n, u8 m);
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

static const int eParamSetTrigId[] =
{
    eParamSetTrig0,
    eParamSetTrig1,
    eParamSetTrig2,
    eParamSetTrig3,
    eParamSetTrig4,
    eParamSetTrig5,
    eParamSetTrig6,
    eParamSetTrig7,
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

static const int eParamOutputLevelId[] =
{
    eParamOutputLevel0,
    eParamOutputLevel1,
    eParamOutputLevel2,
    eParamOutputLevel3,
    eParamOutputLevel4,
    eParamOutputLevel5,
    eParamOutputLevel6,
    eParamOutputLevel7,
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
    if(incAbs < 8) {
        return inc << 2;
    }
    if(incAbs < 32) {
        return inc << 4;
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
    
    //open sample cut per step page
    if(state_sw == 3)
    {
    }
}

void handle_switch_3(s32 data) {
    u8 t = trkpage;
    
    handle_sw(4, data > 0);
    
    //  mute track output
    if(state_sw == 4)
    {
        if (!track[t]->mutetrk)
        {
            track[t]->mutetrk = 1;
            ctl_param_change(DUMMY, eParamFlagId[t], 0);
            ctl_param_change(DUMMY, eParamCurveId[t], 0);
            render_seqtrack(t);
        }
        
        else
        {
            track[t]->mutetrk = 0;
            track_unmute(t, track[t]->m);
            render_seqtrack(t);
        }
    }
}

void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        set_page(ePageTrk);
        render_trk();
    }
    else ;;
}

void handle_encoder_0(s32 val) {
    s32 tmp;
    u8 t = trkpage;
    s32 i = editpos;
    switch (state_sw) {
        case 0: //sample
            check_touch(kEventEncoder3);
            if (touchedThis) {
                if (track[t]->m == 1)
                {
                    tmp = track[t]->s[i];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[t]->s[i] = tmp;
                    ctl_param_change(i, eParamSampleOffsetBId[t], sample[tmp + 1]->offset + (sample[tmp]->loop_cut));
                    ctl_param_change(i, eParamSampleOffsetAId[t], sample[tmp]->offset + sample[tmp]->offset_cut);
                    if (tmp == N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 0);
                    if (tmp != N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 1);
                    scroll_sequence(t);
                    break;
                }
                else if (track[t]->m == 2)
                {
                    tmp = track[t]->s[i];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[t]->s[i] = tmp;
                    ctl_param_change(i, eParamSampleOffsetBId[t], sample[tmp + 1]->offset + (sample[tmp]->loop_cut));
                    ctl_param_change(i, eParamSampleOffsetAId[t], sample[tmp]->offset + sample[tmp]->offset_cut);
                    if (tmp == N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 0);
                    if (tmp != N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 1);
                    scroll_sequence(t);
                    break;
                }
                else if (track[t]->m == 3)
                {
                    tmp = track[t]->e[i];
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > 1) tmp = 1;
                    track[t]->e[i] = tmp;
                    if (tmp == 0) ctl_param_change(i, eParamSetTrigId[t], 0);
                    if (tmp == 1) ctl_param_change(i, eParamSetTrigId[t], 1);
                    scroll_sequence(t);
                    break;
                }
                else ;
            }
            break;
            
        case 1:
            check_touch(kEventEncoder3);
            if (touchedThis) {
                if (track[t]->m == 1)
                {
                    //sample with trig...
                    /*
                    tmp = track[t]->s[i];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[t]->s[i] = tmp;
                    //  edit sequencer
                    ctl_param_change(i, eParamSampleOffsetBId[t], sample[tmp + 1]->offset + (sample[tmp]->loop_cut));
                    ctl_param_change(i, eParamSampleOffsetAId[t], sample[tmp]->offset + sample[tmp]->offset_cut);
                    if (tmp == N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 0);
                    if (tmp != N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 1);
                    //  playback sample
                    ctl_param_change(i, eParamOutputLevelId[t], DEFAULT_LEVEL);
                    ctl_param_change(i, eParamSampleOffsetBId[t], sample[tmp + 1]->offset + (sample[tmp]->loop_cut));
                    ctl_param_change(i, eParamSampleOffsetAId[t], sample[tmp]->offset + sample[tmp]->offset_cut);
                    ctl_param_change(DUMMY, eParamTrigId[t], DUMMY);
                    scroll_sequence(t);
                    break;
                    */
                }
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
            
        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    s32 tmp;
    u8 t = trkpage;
    s32 i = editpos;
    switch (state_sw) {
        case 0:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                if (track[t]->m == 1)
                {
                    //  trig level
                    tmp = track[t]->outL[i];
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
                    track[t]->outL[i] = tmp;
                    ctl_param_change(i, eParamOutputLevelId[t], tmp);
                    scroll_sequence(t);
                    break;
                }
                else if (track[t]->m == 2)
                {
                    //  loop pitch
                    tmp = track[t]->outL[i];
                    tmp += knob_accel(val);
                    if (tmp < 0) tmp = 0;
                    track[t]->outL[i] = tmp;
                    ctl_param_change(i, eParamOutputLevelId[t], tmp);
                    scroll_sequence(t);
                    break;
                }
                else if (track[t]->m == 3)
                {
                    //  envelope duration fine
                    tmp = track[t]->outL[i];
                    tmp += knob_accel(val);
                    if (tmp < 0) tmp = 0;
                    track[t]->outL[i] = tmp;
                    ctl_param_change(i, eParamOutputLevelId[t], tmp);
                    scroll_sequence(t);
                    break;
                }
                else ;
            }
            break;
            
        case 1:
            check_touch(kEventEncoder2);
            if (touchedThis) {
                if (track[t]->m == 2)
                {
                    //  loop pitch coarse
                    tmp = track[t]->outL[i];
                    tmp += val * 65536;
                    if (tmp < 0) tmp = 0;
                    track[t]->outL[i] = tmp;
                    ctl_param_change(i, eParamOutputLevelId[t], tmp);
                    scroll_sequence(t);
                    break;
                }
                //  envelope duration coarse
                else if (track[t]->m == 3)
                {
                    tmp = track[t]->outL[i];
                    tmp += val * 65536;
                    if (tmp < 0) tmp = 0;
                    track[t]->outL[i] = tmp;
                    ctl_param_change(i, eParamOutputLevelId[t], tmp);
                    scroll_sequence(t);
                    break;
                }
                else ;
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
    s32 i = editpos;
    u8 t = trkpage;
    switch (state_sw) {
        case 0: //step
            check_touch(kEventEncoder1);
            if (touchedThis) {
                i += val;
                if (i < 0) i = 0;
                if (i > track[t]->len - 1) i = track[t]->len - 1;
                editpos = i;
                scroll_sequence(t);
            }
            break;
/*
        case 1: //measure
            check_touch(kEventEncoder1);
            if (touchedThis) {
                u32 p;
                if (val < 0)
                {
                    p = i;
                    i -= track[t]->msr;
                    if (i < 0) i = track[t]->len - (track[t]->msr - p);
                    editpos = i;
                    scroll_sequence(t);
                    break;
                }
                else if (val > 0)
                {
                    p = i;
                    i += track[t]->msr;
                    if (i > track[t]->len - 1) i = track[t]->msr - (track[t]->len - p);
                    editpos = i;
                    scroll_sequence(t);
                    break;
                }
                else ;
            }
            break;
*/ 
        case 1: //clone by measure
            check_touch(kEventEncoder1);
            if (touchedThis) {
                u32 p;
                u32 m = track[t]->msr;
                const u32 l = track[t]->len;
                //  remove "even loops" in case measure is longer than length
                while (m > l) { m -= l; }
                s32 clone_a, clone_b;
                if (track[t]->m == 1 || track[t]->m == 2)
                {
                    if (val < 0)
                    {
                        clone_a = track[t]->s[i];
                        clone_b = track[t]->outL[i];
                        p = i;
                        i -= m;
                        if (i < 0) i = l - (m - p);
                        editpos = i;
                        track[t]->s[i] = clone_a;
                        ctl_param_change(i, eParamSampleOffsetBId[t], sample[clone_a + 1]->offset + (sample[clone_a]->loop_cut));
                        ctl_param_change(i, eParamSampleOffsetAId[t], sample[clone_a]->offset + sample[clone_a]->offset_cut);
                        if (clone_a == N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 0);
                        if (clone_a != N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 1);
                        track[t]->outL[i] = clone_b;
                        ctl_param_change(i, eParamOutputLevelId[t], clone_b);
                        scroll_sequence(t);
                        break;
                    }
                    else if (val > 0)
                    {
                        clone_a = track[t]->s[i];
                        clone_b = track[t]->outL[i];
                        p = i;
                        i += m;
                        if (i > l - 1) i = m - (l - p);
                        editpos = i;
                        track[t]->s[i] = clone_a;
                        ctl_param_change(i, eParamSampleOffsetBId[t], sample[clone_a + 1]->offset + (sample[clone_a]->loop_cut));
                        ctl_param_change(i, eParamSampleOffsetAId[t], sample[clone_a]->offset + sample[clone_a]->offset_cut);
                        if (clone_a == N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 0);
                        if (clone_a != N_BUFFERS) ctl_param_change(i, eParamSetTrigId[t], 1);
                        track[t]->outL[i] = clone_b;
                        ctl_param_change(i, eParamOutputLevelId[t], clone_b);
                        scroll_sequence(t);
                        break;
                    }
                    else ;
                }
                else if (track[t]->m == 3)
                {
                    if (val < 0)
                    {
                        clone_a = track[t]->e[i];
                        clone_b = track[t]->outL[i];
                        p = i;
                        i -= m;
                        if (i < 0) i = l - (m - p);
                        editpos = i;
                        track[t]->e[i] = clone_a;
                        if (clone_a == 0) ctl_param_change(i, eParamSetTrigId[t], 0);
                        if (clone_a == 1) ctl_param_change(i, eParamSetTrigId[t], 1);
                        track[t]->outL[i] = clone_b;
                        ctl_param_change(i, eParamOutputLevelId[t], clone_b);
                        scroll_sequence(t);
                        break;
                    }
                    else if (val > 0)
                    {
                        clone_a = track[t]->e[i];
                        clone_b = track[t]->outL[i];
                        p = i;
                        i += m;
                        if (i > l - 1) i = m - (l - p);
                        editpos = i;
                        track[t]->e[i] = clone_a;
                        if (clone_a == 0) ctl_param_change(i, eParamSetTrigId[t], 0);
                        if (clone_a == 1) ctl_param_change(i, eParamSetTrigId[t], 1);
                        track[t]->outL[i] = clone_b;
                        ctl_param_change(i, eParamOutputLevelId[t], clone_b);
                        scroll_sequence(t);
                        break;
                    }
                    else ;
                }
                else ;
            }
            break;
            
        case 2: //shift by step
            check_touch(kEventEncoder1);
            if (touchedThis) {
                u32 p;
                const u32 len = track[t]->len;
                if (track[t]->m == 1 || track[t]->m == 2)
                {
                    //  shift -1
                    if (val < 0 && len > 1)
                    {
                        gpio_clr_gpio_pin(LED_MODE_PIN);
                        //  copy to clone and shift
                        for (p = len - 1; p>0; --p)
                        {
                            clone->c1[p - 1] = track[t]->s[p];
                            clone->c2[p - 1] = track[t]->outL[p];
                        }
                        //  copy first to last
                        clone->c1[len - 1] = track[t]->s[0];
                        clone->c2[len - 1] = track[t]->outL[0];
                        //  refill pattern and send parameter changes
                        for (p = 0; p<len; p++)
                        {
                            s32 tmp = clone->c1[p];
                            track[t]->s[p] = tmp;
                            ctl_param_change(p, eParamSampleOffsetBId[t], sample[tmp + 1]->offset + (sample[tmp]->loop_cut));
                            ctl_param_change(p, eParamSampleOffsetAId[t], sample[tmp]->offset + sample[tmp]->offset_cut);
                            if (tmp == N_BUFFERS) ctl_param_change(p, eParamSetTrigId[t], 0);
                            if (tmp != N_BUFFERS) ctl_param_change(p, eParamSetTrigId[t], 1);
                            track[t]->outL[p] = clone->c2[p];
                            ctl_param_change(p, eParamOutputLevelId[t], clone->c2[p]);
                        }
                        //  align editpos and render pattern
                        --i;
                        if (i < 0) i = len - 1;
                        editpos = i;
                        scroll_sequence(t);
                        gpio_set_gpio_pin(LED_MODE_PIN);
                        break;
                    }
                    //  shift +1
                    else if (val > 0 && len > 1)
                    {
                        gpio_clr_gpio_pin(LED_MODE_PIN);
                        //  copy to clone and shift
                        for (p = 0; p<len; p++)
                        {
                            clone->c1[p + 1] = track[t]->s[p];
                            clone->c2[p + 1] = track[t]->outL[p];
                        }
                        //  copy last to first
                        clone->c1[0] = track[t]->s[len - 1];
                        clone->c2[0] = track[t]->outL[len - 1];
                        //  refill pattern and send parameter changes
                        for (p = 0; p<len; p++)
                        {
                            s32 tmp = clone->c1[p];
                            track[t]->s[p] = tmp;
                            ctl_param_change(p, eParamSampleOffsetBId[t], sample[tmp + 1]->offset + (sample[tmp]->loop_cut));
                            ctl_param_change(p, eParamSampleOffsetAId[t], sample[tmp]->offset + sample[tmp]->offset_cut);
                            if (tmp == N_BUFFERS) ctl_param_change(p, eParamSetTrigId[t], 0);
                            if (tmp != N_BUFFERS) ctl_param_change(p, eParamSetTrigId[t], 1);
                            track[t]->outL[p] = clone->c2[p];
                            ctl_param_change(p, eParamOutputLevelId[t], clone->c2[p]);
                        }
                        //  align editpos and render pattern
                        i++;
                        if (i > len - 1) i = 0;
                        editpos = i;
                        scroll_sequence(t);
                        gpio_set_gpio_pin(LED_MODE_PIN);
                        break;
                    }
                    else ;
                }
                else if (track[t]->m == 3)
                {
                    //  shift -1
                    if (val < 0 && len > 1)
                    {
                        gpio_clr_gpio_pin(LED_MODE_PIN);
                        //  copy to clone and shift
                        for (p = len - 1; p>0; --p)
                        {
                            clone->c1[p - 1] = track[t]->e[p];
                            clone->c2[p - 1] = track[t]->outL[p];
                        }
                        //  copy first to last
                        clone->c1[len - 1] = track[t]->e[0];
                        clone->c2[len - 1] = track[t]->outL[0];
                        //  refill pattern and send parameter changes
                        for (p = 0; p<len; p++)
                        {
                            s32 tmp = clone->c1[p];
                            track[t]->e[p] = tmp;
                            if (tmp == 0) ctl_param_change(p, eParamSetTrigId[t], 0);
                            if (tmp == 1) ctl_param_change(p, eParamSetTrigId[t], 1);
                            track[t]->outL[p] = clone->c2[p];
                            ctl_param_change(p, eParamOutputLevelId[t], clone->c2[p]);
                        }
                        //  align editpos and render pattern
                        --i;
                        if (i < 0) i = len - 1;
                        editpos = i;
                        scroll_sequence(t);
                        gpio_set_gpio_pin(LED_MODE_PIN);
                        break;
                    }
                    //  shift +1
                    else if (val > 0 && len > 1)
                    {
                        gpio_clr_gpio_pin(LED_MODE_PIN);
                        //  copy pattern to clone and shift
                        for (p = 0; p<len; p++)
                        {
                            clone->c1[p + 1] = track[t]->e[p];
                            clone->c2[p + 1] = track[t]->outL[p];
                        }
                        //  copy last to first
                        clone->c1[0] = track[t]->e[len - 1];
                        clone->c2[0] = track[t]->outL[len - 1];
                        //  refill pattern and send parameter changes
                        for (p = 0; p<len; ++p)
                        {
                            s32 tmp = clone->c1[p];
                            track[t]->e[p] = tmp;
                            if (tmp == 0) ctl_param_change(p, eParamSetTrigId[t], 0);
                            if (tmp == 1) ctl_param_change(p, eParamSetTrigId[t], 1);
                            track[t]->outL[p] = clone->c2[p];
                            ctl_param_change(p, eParamOutputLevelId[t], clone->c2[p]);
                        }
                        //  align editpos and render pattern
                        i++;
                        if (i > len - 1) i = 0;
                        editpos = i;
                        scroll_sequence(t);
                        gpio_set_gpio_pin(LED_MODE_PIN);
                        break;
                    }
                    else ;
                }
                else ;
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
    u8 t = trkpage;
    s32 i = editpos;
    switch (state_sw) {
        case 0: //track
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = trkpage;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_TRACKS_1) tmp = N_TRACKS_1;
                trkpage = tmp;
                scroll_sequence(tmp);
                render_seqtrack(tmp);
            }
            break;
            
        case 1: //measure
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[t]->msr;
                tmp += val;
                if (tmp < 1) tmp = 1;
                if (tmp > SQ_LEN) tmp = SQ_LEN;
                track[t]->msr = tmp;
                render_seqtrack(t);
            }
            break;
            
        case 2: //length
            check_touch(kEventEncoder0);
            if (touchedThis) {
                tmp = track[t]->len;
                tmp += val;
                if (tmp < 1) tmp = 1;
                if (tmp > SQ_LEN) tmp = SQ_LEN;
                if (i > tmp - 1) i = tmp - 1;
                track[t]->len = tmp;
                editpos = i;
                ctl_param_change(i, eParamLengthId[t], tmp);
                scroll_sequence(t);
                render_seqtrack(t);
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

void track_unmute(u8 n, u8 m) {
    prgmTrack *t = track[n];
    
    //  OFF
    if(m == 0) ;
    
    //  trig
    else if(m == 1)
    {
        t->m = 1;
        ctl_param_change(DUMMY, eParamFlagId[n], 1);
        ctl_param_change(DUMMY, eParamCurveId[n], 1);
    }
    
    //  loop
    else if(m == 2)
    {
        ctl_param_change(DUMMY, eParamFlagId[n], 2);
        ctl_param_change(DUMMY, eParamCurveId[n], 2);
    }
    
    //  [amp]
    else if(m == 3)
    {
        ctl_param_change(DUMMY, eParamFlagId[n], 2);
        ctl_param_change(DUMMY, eParamCurveId[n], t->env);
    }
    
    //  [dly]
    else if(m == 4)
    {
        ctl_param_change(DUMMY, eParamFlagId[n], 3);
        ctl_param_change(DUMMY, eParamCurveId[n], 8);
    }
    
    //  [thru]
    else if(m == 5)
    {
        t->m = 5;
        ctl_param_change(DUMMY, eParamFlagId[n], 4);
        ctl_param_change(DUMMY, eParamCurveId[n], 9);
    }
}

void select_seq(void) {
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
