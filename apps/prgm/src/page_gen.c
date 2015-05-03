//page_param.c
//aleph-prgm-avr32

#include "page_gen.h"

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

static void set_mode(u8 track, u8 mode, u32 editpos);


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

static const etype enctypes[4] = {
    kEventEncoder0,
    kEventEncoder1,
    kEventEncoder2,
    kEventEncoder3
};

//render
void handle_switch_3(s32 data) {
    handle_sw(4, data > 0);
}

/*
    static event_t e;
    u8 t = trackpos;

    
    if(state_sw == 4)
    {
        s32 tmp;
        u32 p;
        for (p = 0; p<256; p++)
        {
            if (editpos > track[t]->len - 1) editpos = 0;
            tmp = table_lookup(p);
            e.type = enctypes[t];
            e.data = tmp;
            event_post(&e);
            editpos++;
        }
            print_fix16(renderEditPos, (editpos + 1) * FIX16_ONE);
            render_gsdest(7);
            render_gscounters();
        }
}
*/

//flip page to mix
void handle_switch_4(s32 data) {
    handle_sw(5, data > 0);
    
    if(state_sw == 5)
    {
        set_page(ePageTrk);
        render_trk();
    }
    else ;;
}


/*
 tmp = track[t]->len;
 tmp += val;
 if (tmp < 1) tmp = 1;
 if (tmp > SQ_LEN) tmp = SQ_LEN;
 track[t]->len = tmp;
 ctl_param_change(DUMMY, eParamLengthId[t], tmp);
 render_track(t);
*/

void handle_encoder_0(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0:
            //pos
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = genpos;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_GEN_1) tmp = N_GEN_1;
                genpos = tmp;
                print_fix16(renderGenPos, (tmp + 1) * FIX16_ONE);
                print_fix16(renderGenDens, gen->gDen[tmp] * FIX16_ONE);
                print_fix16(renderGenDur, gen->gDur[tmp] * FIX16_ONE);
                print_fix16(renderEnvPhase, gen->env[tmp].phase);
                print_fix16(renderEnvFreq, gen->env[tmp].f);
                print_fix16(renderEnvRate, gen->env[tmp].sr * FIX16_ONE);
                print_fix16(renderEnvAtten, gen->env[tmp].att);
                render_gen_param(tmp);
                render_gscounters();
            }
            break;
                    
        case 1: //env trip point
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = gen->env[genpos].trip;
                tmp += val;
                if (tmp < 0) tmp = 0;
                gen->env[genpos].trip = tmp;
                print_fix16(renderEnvPhase, tmp);
                render_gen_param(genpos);
            }
            break;
            
        case 2: //env sample rate
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = gen->env[genpos].sr;
                tmp *= val * 2;
                if (tmp < 2) tmp = 2;
                if (tmp > 48000) tmp = 48000;
                gen->env[genpos].sr = tmp;
                print_fix16(renderEnvRate, tmp * FIX16_ONE);
                render_gen_param(genpos);
            }
            break;
            
        case 3: //gen density
            check_touch(kEventEncoder3);
            if (touchedThis) {
                tmp = gen->gDen[genpos];
                tmp += val;
                if (tmp < 1) tmp = 1;
                if (tmp > 1024) tmp = 1024;
                gen->gDen[genpos] = tmp;
                print_fix16(renderGenDens, tmp * FIX16_ONE);
                render_gen_param(genpos);
            }
            break;
                        
        default:
            break;
    }
}

void handle_encoder_1(s32 val) {
    s32 tmp;
    switch (state_sw) {
        case 0: //envelope shape
            check_touch(kEventEncoder2);
            if (touchedThis) {
//                tmp = genpos;
//                tmp += val;
//                if (tmp < 0) tmp = 0;
//                if (tmp > N_ENV_1) tmp = N_ENV_1;
//                gen->env[genpos] = tmp;
//                render_gen_param(genpos);
            }
            break;
            
        case 1: //frequency
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = gen->env[genpos].f;
                tmp += val * FIX16_ONE;
                if (tmp < 0) tmp = 0;
                env_set_f(&(gen->env[genpos]), tmp);
                print_fix16(renderEnvFreq, tmp);
                render_gen_param(genpos);
            }
            break;
            
        case 2: //attenuation
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = gen->env[genpos].att;
                tmp += val * 4194304;
                if (tmp < 0) tmp = 0;
                gen->env[genpos].att = tmp;
                print_fix16(renderEnvAtten, tmp);
                render_gen_param(genpos);
            }
            break;
            
        case 3: //duration
            check_touch(kEventEncoder2);
            if (touchedThis) {
                tmp = gen->gDur[genpos];
                tmp += val;
                if (tmp < 1) tmp = 1;
                if (tmp > 1024) tmp = 1024;
                gen->gDur[genpos] = tmp;
                print_fix16(renderGenDur, tmp * FIX16_ONE);
                render_gen_param(genpos);
            }
            break;
                        
        default:
            break;
    }
}

void handle_encoder_2(s32 val) {
    s32 tmp;
    s32 i = editpos;
    u8 s;
    switch (state_sw) {
        case 0: //track
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = trackpos;
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_TRACKS_1) tmp = N_TRACKS_1;
                trackpos = tmp;
                render_gstrackname(tmp);
                render_gsdest(gen->gD[genpos]);
            }
            break;
    
        case 1: //gen destination
            check_touch(kEventEncoder1);
            if (touchedThis) {
                tmp = gen->gD[genpos];
                tmp += val;
                if (tmp < 0) tmp = 0;
                if (tmp > N_DST_1) tmp = N_DST_1;
                gen->gD[genpos] = tmp;
                render_gsdest(tmp);
            }
            break;
            
        case 2: //clone with density
            check_touch(kEventEncoder1);
            if (touchedThis) {
                u8 d = gen->gD[genpos];
                u8 t = trackpos;
                u32 p;
                s32 clone;
                if (d == 0)
                {
                    if (val < 0)
                    {
//                        clone = track[t]->cTG[i];
                        p = i;
                        i -= gen->gDen[genpos];
                        if (i < 0) i = track[t]->len - (gen->gDen[genpos] - p);
                        editpos = i;
//                        track[t]->cTG[i] = clone;
                        ctl_param_change(i, eParamTrigId[t], clone);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
//                        clone = track[t]->cTG[i];
                        p = i;
                        i += gen->gDen[genpos];
                        if (i > track[t]->len - 1) i = gen->gDen[genpos] - (track[t]->len - p);
                        editpos = i;
//                        track[t]->cTG[i] = clone;
                        ctl_param_change(i, eParamTrigId[t], clone);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 1)
                {
                    if (val < 0)
                    {
//                        clone = track[t]->cT[i];
                        p = i;
                        i -= gen->gDen[genpos];
                        if (i < 0) i = track[t]->len - (gen->gDen[genpos] - p);
                        editpos = i;
//                        track[t]->cT[i] = clone;
//                        ctl_param_change(i, eParamTimeId[t], clone);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
//                        clone = track[t]->cT[i];
                        p = i;
                        i += gen->gDen[genpos];
                        if (i > track[t]->len - 1) i = gen->gDen[genpos] - (track[t]->len - p);
                        editpos = i;
//                        track[t]->cT[i] = clone;
//                        ctl_param_change(i, eParamTimeId[t], clone);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 2)
                {
                    if (val < 0)
                    {
                        clone = track[t]->m;
                        p = i;
                        i -= gen->gDen[genpos];
                        if (i < 0) i = track[t]->len - (gen->gDen[genpos] - p);
                        editpos = i;
                        track[t]->m = clone;
                        set_mode(t, clone, i);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
                        clone = track[t]->m;
                        p = i;
                        i += gen->gDen[genpos];
                        if (i > track[t]->len - 1) i = gen->gDen[genpos] - (track[t]->len - p);
                        editpos = i;
                        track[t]->m = clone;
                        set_mode(t, clone, i);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;

                }
                else if (d == 3)
                {
                    if (val < 0)
                    {
                        clone = track[t]->s[i];
                        p = i;
                        i -= gen->gDen[genpos];
                        if (i < 0) i = track[t]->len - (gen->gDen[genpos] - p);
                        editpos = i;
                        track[t]->s[i] = clone;
//                        ctl_param_change(i, eParamLoopId[t], sample[clone]->loop);
                        ctl_param_change(i, eParamOffsetAId[t], sample[clone]->offset);
//                        ctl_param_change(i, eParamTimeId[t], sample[clone]->loop);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
                        clone = track[t]->s[i];
                        p = i;
                        i += gen->gDen[genpos];
                        if (i > track[t]->len - 1) i = gen->gDen[genpos] - (track[t]->len - p);
                        editpos = i;
                        track[t]->s[i] = clone;
//                        ctl_param_change(i, eParamLoopId[t], sample[clone]->loop);
                        ctl_param_change(i, eParamOffsetAId[t], sample[clone]->offset);
//                        ctl_param_change(i, eParamTimeId[t], sample[clone]->loop);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 4)
                {
                    ;
                }
                else if (d == 5)
                {
                    //delay
                    if (track[t]->m == 6)
                    {
                        if (val < 0)
                        {
                            clone = track[t]->uP;
                            p = i;
                            i -= gen->gDen[genpos];
                            if (i < 0) i = track[t]->len - (gen->gDen[genpos] - p);
                            editpos = i;
                            if (clone < DLOOP_MIN) clone = DLOOP_MIN;
                            if (clone > AUX_SIZE_1) clone = AUX_SIZE_1;
                            track[t]->uP = clone;
//                            ctl_param_change(i, eParamLoopId[t], clone);
                            print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                            render_gsdest(d);
                            render_gscounters();
                            break;
                        }
                        else if (val > 0)
                        {
                            clone = track[t]->uP;
                            p = i;
                            i += gen->gDen[genpos];
                            if (i > track[t]->len - 1) i = gen->gDen[genpos] - (track[t]->len - p);
                            editpos = i;
                            if (clone < DLOOP_MIN) clone = DLOOP_MIN;
                            if (clone > AUX_SIZE_1) clone = AUX_SIZE_1;
                            track[t]->uP = clone;
//                            ctl_param_change(i, eParamLoopId[t], clone);
                            print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                            render_gsdest(d);
                            render_gscounters();
                            break;
                        }
                        else ;
                    }
                }
                else if (d == 6)
                {
                    if (val < 0)
                    {
//                        clone = track[t]->eL[i];
                        p = i;
                        i -= gen->gDen[genpos];
                        if (i < 0) i = track[t]->len - (gen->gDen[genpos] - p);
                        editpos = i;
//                        track[t]->eL[i] = clone;
                        ctl_param_change(i, eParamOutputLevelId[t], clone);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
//                        clone = track[t]->eL[i];
                        p = i;
                        i += gen->gDen[genpos];
                        if (i > track[t]->len - 1) i = gen->gDen[genpos] - (track[t]->len - p);
                        editpos = i;
//                        track[t]->eL[i] = clone;
                        ctl_param_change(i, eParamOutputLevelId[t], clone);
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 7)
                {
                    ;
                }
                else ;
            }
            break;
            
        case 4: //render with density
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
    s32 i = editpos;
    u8 d, s, t;
    switch (state_sw) {
        case 0: //edit position
            check_touch(kEventEncoder0);
            if (touchedThis) {
                t = trackpos;
                i += val;
                if (i < 0) i = track[t]->len - 1;
                if (i > track[t]->len - 1) i = 0;
                editpos = i;
                print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                render_gsdest(gen->gD[genpos]);
                render_gscounters();
            }
            break;
            
        case 1: //destination value
            check_touch(kEventEncoder0);
            if (touchedThis) {
                d = gen->gD[genpos];
                t = trackpos;
                //trig
                if (d == 0)
                {
//                    tmp = track[t]->cTG[i];
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > 1) tmp = 1;
//                    track[t]->cTG[i] = tmp;
                    ctl_param_change(i, eParamTrigId[t], tmp);
                    render_gsdest(d);
                }
                
                //time
                else if (d == 1)
                {
//                    tmp = track[t]->cT[i];
                    tmp += knob_accel(val);
                    if (tmp < 0) tmp = 0;
//                    track[t]->cT[i] = tmp;
//                    ctl_param_change(i, eParamTimeId[t], tmp);
                    render_gsdest(d);
                }
                
                //mode
                else if (d == 2)
                {
                    tmp = track[t]->m;
                    tmp += val;
                    if (tmp < 0) tmp = 0;
                    if (tmp > N_MODES_1) tmp = N_MODES_1;
//                    set_mode(t, tmp, i);
                    render_gsdest(d);
                }
                
                //wav
                else if (d == 3)
                {
                    tmp = track[t]->s[i];
                    tmp += val;
                    if (tmp < N_BUFFERS) tmp = N_BUFFERS;
                    if (tmp > n_samples - 1) tmp = n_samples - 1;
                    track[t]->s[i] = tmp;
                    ctl_param_change(i, eParamOffsetBId[t], sample[tmp + 1]->offset);
                    ctl_param_change(i, eParamOffsetAId[t], sample[tmp]->offset);
                    render_gsdest(d);
                }
                
                //offsetA
                else if (d == 4)
                {
                    ;
                }
                //offsetB
                else if (d == 5)
                {
                    ;
                }
                //level
                else if (d == 6)
                {
//                    tmp = track[t]->eL[i];
                    tmp += val * 4194304;
                    if (tmp < 0) tmp = 0;
//                    track[t]->eL[i] = tmp;
                    ctl_param_change(i, eParamOutputLevelId[t], tmp);
                    render_gsdest(d);
                }
                //frequency
                else if (d == 7)
                {
                    ;
                }
                else ;
            }
            break;
            
        case 2: //clone with duration
            check_touch(kEventEncoder0);
            if (touchedThis) {
                u8 d = gen->gD[genpos];
                u8 t = trackpos;
                u32 p;
                s32 clone;
                if (d == 0)
                {
                    if (val < 0)
                    {
//                        clone = track[t]->cTG[i];
                        for (p = 0; p<gen->gDur[genpos]; ++p)
                        {
                            --i;
                            if (i < 0) i = track[t]->len - 1;
//                            track[t]->cTG[i] = clone;
                            ctl_param_change(i, eParamTrigId[t], clone);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }                    
                    else if (val > 0)
                    {
//                        clone = track[t]->cTG[i];
                        for (p = 0; p<gen->gDur[genpos]; p++)
                        {
                            i++;
                            if (i > track[t]->len - 1) i = 0;
//                            track[t]->cTG[i] = clone;
                            ctl_param_change(i, eParamTrigId[t], clone);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 1)
                {
                    if (val < 0)
                    {
//                        clone = track[t]->cT[i];
                        for (p = 0; p<gen->gDur[genpos]; ++p)
                        {
                            --i;
                            if (i < 0) i = track[t]->len - 1;
//                            track[t]->cT[i] = clone;
//                            ctl_param_change(i, eParamTimeId[t], clone);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
//                        clone = track[t]->cT[i];
                        for (p = 0; p<gen->gDur[genpos]; p++)
                        {
                            i++;
                            if (i > track[t]->len - 1) i = 0;
//                            track[t]->cT[i] = clone;
//                            ctl_param_change(i, eParamTimeId[t], clone);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 2)
                {
                    if (val < 0)
                    {
                        clone = track[t]->m;
                        for (p = 0; p<gen->gDur[genpos]; ++p)
                        {
                            --i;
                            if (i < 0) i = track[t]->len - 1;
                            track[t]->m = clone;
                            set_mode(t, clone, i);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
                        clone = track[t]->m;
                        for (p = 0; p<gen->gDur[genpos]; p++)
                        {
                            i++;
                            if (i > track[t]->len - 1) i = 0;
                            track[t]->m = clone;
                            set_mode(t, clone, i);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 3)
                {
                    if (val < 0)
                    {
                        clone = track[t]->s[i];
                        for (p = 0; p<gen->gDur[genpos]; ++p)
                        {
                            --i;
                            if (i < 0) i = track[t]->len - 1;
                            track[t]->s[i] = clone;
//                            ctl_param_change(i, eParamLoopId[t], sample[clone]->loop);
                            ctl_param_change(i, eParamOffsetAId[t], sample[clone]->offset);
//                            ctl_param_change(i, eParamTimeId[t], sample[clone]->loop);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
                        clone = track[t]->s[i];
                        for (p = 0; p<gen->gDur[genpos]; p++)
                        {
                            i++;
                            if (i > track[t]->len - 1) i = 0;
                            track[t]->s[i] = clone;
//                            ctl_param_change(i, eParamLoopId[t], sample[clone]->loop);
                            ctl_param_change(i, eParamOffsetAId[t], sample[clone]->offset);
//                            ctl_param_change(i, eParamTimeId[t], sample[clone]->loop);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 4)
                {
                    ;
                }
                else if (d == 5)
                {
                    //delay
                    if (track[t]->m == 6)
                    {
                        if (val < 0)
                        {
                            clone = track[t]->uP;
                            for (p = 0; p<gen->gDur[genpos]; ++p)
                            {
                                --i;
                                if (i < 0) i = track[t]->len - 1;
                                if (clone < DLOOP_MIN) clone = DLOOP_MIN;
                                if (clone > AUX_SIZE_1) clone = AUX_SIZE_1;
                                track[t]->uP = clone;
//                                ctl_param_change(i, eParamLoopId[t], clone);
                            }
                            editpos = i;
                            print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                            render_gsdest(d);
                            render_gscounters();
                            break;
                        }
                        else if (val > 0)
                        {
                            clone = track[t]->uP;
                            for (p = 0; p<gen->gDur[genpos]; p++)
                            {
                                i++;
                                if (i > track[t]->len - 1) i = 0;
                                if (clone < DLOOP_MIN) clone = DLOOP_MIN;
                                if (clone > AUX_SIZE_1) clone = AUX_SIZE_1;
                                track[t]->uP = clone;
//                                ctl_param_change(i, eParamLoopId[t], clone);
                            }
                            editpos = i;
                            print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                            render_gsdest(d);
                            render_gscounters();
                            break;
                        }
                        else ;
                    }
                }
                else if (d == 6)
                {
                    if (val < 0)
                    {
//                        clone = track[t]->eL[i];
                        for (p = 0; p<gen->gDur[genpos]; ++p)
                        {
                            --i;
                            if (i < 0) i = track[t]->len - 1;
//                            track[t]->eL[i] = clone;
                            ctl_param_change(i, eParamOutputLevelId[t], clone);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else if (val > 0)
                    {
//                        clone = track[t]->eL[i];
                        for (p = 0; p<gen->gDur[genpos]; p++)
                        {
                            i++;
                            if (i > track[t]->len - 1) i = 0;
//                            track[t]->eL[i] = clone;
                            ctl_param_change(i, eParamOutputLevelId[t], clone);
                        }
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(d);
                        render_gscounters();
                        break;
                    }
                    else ;
                }
                else if (d == 7)
                {
                    ;
                }
                else ;
            }
            break;
            
        case 4: //render with duration
            check_touch(kEventEncoder0);
            if (touchedThis) {
                u8 d = 6;
                u8 t = trackpos;
                u32 p;
                if (d == 6)
                {
                    if (val)
                    {
                        gpio_clr_gpio_pin(LED_MODE_PIN);
                        //set env waveform + pitch + samplerate + tripPoint + attenuation
                        //add density parameter
                        env_set_trig(1);
                        for (p = 0; p<gen->gDur[genpos]; p++)
                        {
                            i++;
                            if (i > track[t]->len - 1) i = 0;
                            tmp = env_next(&(gen->env[genpos]));
//                            track[t]->eL[i] = tmp;
                            ctl_param_change(i, eParamOutputLevelId[t], tmp);
                        }
                        env_set_trig(0);
                        editpos = i;
                        print_fix16(renderEditPos, (i + 1) * FIX16_ONE);
                        render_gsdest(gen->gD[genpos]);
                        render_gscounters();
                        gpio_set_gpio_pin(LED_MODE_PIN);
                        break;
                    }
                }
                else ;
            }
            break;

        default:
            break;
    }
}

void set_mode(u8 n, u8 m, u32 i) {
    prgmTrack *t = track[n];
    
    //  OFF
    if(m == 0)
    {
        t->m = 0;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 0);
    }
    
    //  TRIG
    else if(m == 1)
    {
        t->m = 1;
        ctl_param_change(DUMMY, eParamFlagId[n], 5);
        ctl_param_change(DUMMY, eParamCurveId[n], 13);
    }
    
    //  GATE
    else if(m == 2)
    {
        t->m = 2;
        ctl_param_change(DUMMY, eParamFlagId[n], 5);
        ctl_param_change(DUMMY, eParamCurveId[n], 14);
    }
    
    //  TCD
    else if(m == 3)
    {
        t->m = 3;
        ctl_param_change(DUMMY, eParamFlagId[n], 5);
        ctl_param_change(DUMMY, eParamCurveId[n], 15);
    }
    
    //  wav
    else if(m == 4)
    {
        t->m = 4;
        t->s[i] = 8;
        ctl_param_change(DUMMY, eParamFlagId[n], 2);
        ctl_param_change(DUMMY, eParamCurveId[n], 1);
//        ctl_param_change(i, eParamLoopId[n], sample[8]->loop);
        ctl_param_change(i, eParamOffsetAId[n], sample[8]->offset);
        ctl_param_change(i, eParamOffsetBId[n], sample[9]->offset);
    }
    
    //  noise
    else if(m == 5)
    {
        t->m = 5;
        ctl_param_change(DUMMY, eParamFlagId[n], 2);
        ctl_param_change(DUMMY, eParamCurveId[n], 16);
    }
    
    //  play
    else if(m == 6)
    {
        t->m = 6;
        ctl_param_change(DUMMY, eParamFlagId[n], 1);
        ctl_param_change(DUMMY, eParamCurveId[n], 1);
//        ctl_param_change(i, eParamLoopId[n], sample[n]->loop);
        ctl_param_change(i, eParamOffsetAId[n], sample[n]->offset);
    }
    
    //  rec
    else if(m == 7)
    {
        t->m = 7;
        ctl_param_change(DUMMY, eParamFlagId[n], 1);
        ctl_param_change(DUMMY, eParamCurveId[n], 2);
//        ctl_param_change(i, eParamLoopId[n], sample[n]->loop);
//        ctl_param_change(i, eParamOffsetId[n], sample[n]->offset);
    }
    
    //  TRIGrec
    else if(m == 8)
    {
        t->m = 8;
        ctl_param_change(DUMMY, eParamFlagId[n], 5);
        ctl_param_change(DUMMY, eParamCurveId[n], 3);
//        ctl_param_change(i, eParamLoopId[n], sample[n]->loop);
//        ctl_param_change(i, eParamOffsetId[n], sample[n]->offset);
    }
    
    //  [bp]
    else if(m == 9)
    {
        t->m = 9;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 4);
    }
    
    //  [delay]
    else if(m == 10)
    {
        t->m = 10;
        ctl_param_change(DUMMY, eParamFlagId[n], 4);
        ctl_param_change(DUMMY, eParamCurveId[n], 5);
//        ctl_param_change(i, eParamLoopId[n], sample[n+4]->loop);
//        ctl_param_change(i, eParamOffsetId[n], sample[n+4]->offset);
    }
    
    //  [bp->fb(loop)]
    else if(m == 11)
    {
        t->m = 11;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 6);
    }
    
    //  [fb(loop->bp)]
    else if(m == 12)
    {
        t->m = 12;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 7);
    }
    
    //  [fb(diffuse)]
    else if(m == 13)
    {
        t->m = 13;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 8);
    }
    
    //  [diffuse->bp]
    else if(m == 14)
    {
        t->m = 14;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 9);
    }
    
    //  [3bp]
    else if(m == 15)
    {
        t->m = 15;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 10);
    }
    
    //  [insert mix]
    else if(m == 16)
    {
        t->m = 16;
        ctl_param_change(DUMMY, eParamFlagId[n], 3);
        ctl_param_change(DUMMY, eParamCurveId[n], 11);
    }
    
    //  [aux master]
    else if(m == 17)
    {
        t->m = 17;
        ctl_param_change(DUMMY, eParamFlagId[n], 1);
        ctl_param_change(DUMMY, eParamCurveId[n], 12);
    }
    
    //  <LEN>
    else if(m == 18)
    {
        t->m = 18;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 0);
    }
    
    //  <EXT>
    else if(m == 19)
    {
        t->m = 19;
        ctl_param_change(DUMMY, eParamFlagId[n], 0);
        ctl_param_change(DUMMY, eParamCurveId[n], 0);
    }
    
    else ;
}


void select_gen(void) {
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
