//prgm.c, aleph-bfin
#include "prgm.h"

//data types
typedef struct _prgmData {
    ModuleData super;
    ParamData mParamData[eParamNumParams];
    
    //audio buffer
    volatile fract32 sampleBuffer[BUFFER_SIZE];

} prgmData;

ModuleData *gModuleData;

//pointers to SDRAM (all external memory)
prgmData *data;

//sequencer
u32 counter;
u32 substep; //TEST CLK MODE
u32 step; //TEST CLK MODE

//audio buffer
sampleBuffer onchipbuffer[N_TRACKS];
u32 offset;

//sequencer
prgmSq *sq[N_TRACKS];

//tracks
prgmTrack *track[N_TRACKS];

//master track
prgmMaster *master;


//function declarations
//memory allocation
static PrgmSqptr init_sq(void);
static void init_sq_parameters(prgmSq *sq);
static PrgmTrackptr init_track(void);
static void init_track_parameters(prgmTrack *t);
static PrgmMasterptr init_master(void);
static void init_master_parameters(void);

//process frame
static fract32 (*get_processptr(u8 n))();
static fract32 pf_off(prgmTrack *t);
static fract32 pf_audio(prgmTrack *t);
static fract32 pf_amp(prgmTrack *t);
static fract32 pf_delay(prgmTrack *t);
static fract32 pf_thru(prgmTrack *t);
static fract32 pf_tape(prgmTrack *t);
static fract32 pf_cue(prgmTrack *t);
static fract32 pf_mute(prgmTrack *t);
static fract32 pf_holdmute(prgmTrack *t);
static fract32 pf_unmute(prgmTrack *t);
static fract32 pf_clock(prgmTrack *t); //TEST CLK MODE
static void route_mix(s32 t, s32 r);
static void track_set_output(prgmTrack *t, fract32 out);
static void set_output3(u8 n);
static void set_output4(u8 n);
static fract32 out_off(void);
static fract32 out_vout0(void);
static fract32 out_vout1(void);
static fract32 out_vout2(void);
static fract32 out_vout3(void);
static fract32 out_vout4(void);
static fract32 out_vout5(void);
static fract32 out_vout6(void);
static fract32 out_vout7(void);
static fract32 out_aux(void);
static fract32 (*get_outputptr(u8 n))();


//events
static void param_reset_counter(void);


PrgmSqptr init_sq(void) {
    return(PrgmSqptr)malloc(sizeof(prgmSq));
}


void init_sq_parameters(prgmSq *sq) {
    u8 i;
    
    for (i=0; i<SQ_LEN; i++)
    {
        sq->tg[i] = 0;
        sq->a[i] = 0;
        sq->b[i] = 0;
        sq->l[i] = 0;
    }
}


PrgmTrackptr init_track(void) {
    return(PrgmTrackptr)malloc(sizeof(prgmTrack));
}

void init_track_parameters(prgmTrack *t) {
    t->flag = 0;
    t->process = get_processptr(0);
    t->output = 0;

    t->mute = 0;
    t->hold = 0;
    
    t->uP = 512;
    t->sP = 0;
    
    t->to_mix = 1;
    t->to_grp1 = 0;
    t->to_grp2 = 0;
    
    t->mix = 0;
    t->aux = 0;

    t->len = SQ_LEN;
    t->c = 0;
    
    filter_1p_lo_init(&(t->pSlew), 0xf);
    filter_1p_lo_set_slew(&(t->pSlew), 0x3f000000); //parameter slew
    filter_1p_lo_init(&(t->mSlew), 0xf);
    filter_1p_lo_set_slew(&(t->mSlew), 0x7f000000); //mix level slew
    
    env_tcd_init(&(t->envAmp));
}


PrgmMasterptr init_master(void) {
    return(PrgmMasterptr)malloc(sizeof(prgmMaster));
}

void init_master_parameters() {
    master->output3 = get_outputptr(0);
    master->output4 = get_outputptr(0);
    master->direct = FR32_MAX;
    
    master->grp1 = FR32_MAX;
    master->grp2 = FR32_MAX;
    master->output = FR32_MAX;
}


static inline void param_setup(u32 id, ParamValue v) {
    gModuleData->paramData[id].value = v;
//    module_set_param(id, v);
}


void module_init(void) {
    data = (prgmData*)SDRAM_ADDRESS;
    gModuleData = &(data->super);
    strcpy(gModuleData->name, "aleph-prgm");
    gModuleData->paramData = data->mParamData;
    gModuleData->numParams = eParamNumParams;
    
    u32 i, n;
    
    //  init sequencer
    for(n=0; n<N_TRACKS; n++)
    {
        sq[n] = init_sq();
        init_sq_parameters(sq[n]);
    }
    
    //  init tracks
    for(n=0; n<N_TRACKS; n++)
    {
        track[n] = init_track();
        init_track_parameters(track[n]);
    }
    
    //  init master
    master = init_master();
    init_master_parameters();
    
    //  init buffers
    for(n=0; n<N_TRACKS; n++)
    {
        buffer_init(&(onchipbuffer[n]), data->sampleBuffer, BUFFER_SIZE);
    }
    
    //  init track playheads
    for(n=0; n<N_TRACKS; n++)
    {
        for (i=0; i<N_HEADS; i++)
        {
            buffer_head_init(&(track[n])->envAmp.head[i], &(onchipbuffer[n]));
        }
    }
    
    //  zero buffer
    for(i=0; i<BUFFER_SIZE; i++) data->sampleBuffer[i] = 0x00000000;
    
    //  init static variables
    offset = 0;
    counter = 0;
    substep = 0;
    step = 0x1770; //CLK MODE TEST, 120 bpm
    
    //  init parameters
    for(i=0; i<module_get_num_params(); i++) param_setup(i, 0);
}


void module_deinit(void) {}


extern u32 module_get_num_params(void) {
    return eParamNumParams;
}


void module_load_sample(s32 sample) {
}


//process frame
fract32 (*get_processptr(u8 n))() {
    static fract32 (*process[])() =
    {
        pf_off,                         //0
        pf_audio,                       //1
        pf_amp,                         //2
        pf_delay,                       //3
        pf_thru,                        //4
        pf_tape,                        //5
        pf_cue,                         //6
        pf_mute,                        //7
        pf_holdmute,                    //8
        pf_unmute,                      //9
        pf_clock,                       //10
    };

    return (n < 1 || n > 11) ? *process[0] : *process[n];
}


void module_process_frame(void) {
    u8 i;
    fract32 tmp, a, b;
    
    tmp = 0x00000000;
    
    //  mix aux
    for(i=0;i<N_TRACKS;i++)
    {
        if (track[i]->aux) tmp = add_fr1x32(tmp, mult_fr1x32x32(vout[i], track[i]->aux));
        else ;
    }
    aux = tmp;
    
    //  process audio
    a = b = tmp = 0x00000000;
    
    for(i=0;i<N_TRACKS;i++)
    {
        fract32 v = 0x00000000;
        //  skip inactive tracks
        if(!track[i]->flag)
        {
            vout[i] = 0x00000000;
        }
        else
        {
            //  calculate active tracks
            vout[i] = 0x00000000;
            v = vout[i] = track[i]->process(track[i]);
        }
        //  mix
        if(track[i]->to_mix)
        {
            if (filter_1p_sync(&(track[i]->mSlew))) tmp = add_fr1x32(tmp, mult_fr1x32x32(v, track[i]->mix));
            else tmp = add_fr1x32(tmp, mult_fr1x32x32(v, filter_1p_lo_next(&(track[i]->mSlew))));
        }
        //  group 1
        if(track[i]->to_grp1)
        {
            if (filter_1p_sync(&(track[i]->mSlew))) a = add_fr1x32(a, mult_fr1x32x32(v, track[i]->mix));
            else a = add_fr1x32(a, mult_fr1x32x32(v, filter_1p_lo_next(&(track[i]->mSlew))));
        }
        //  group 2
        if(track[i]->to_grp2)
        {
            if (filter_1p_sync(&(track[i]->mSlew))) b = add_fr1x32(b, mult_fr1x32x32(v, track[i]->mix));
            else b = add_fr1x32(b, mult_fr1x32x32(v, filter_1p_lo_next(&(track[i]->mSlew))));
        }
    }

    //  add groups to mix
    tmp = add_fr1x32(tmp, mult_fr1x32x32(a, master->grp1));
    tmp = add_fr1x32(tmp, mult_fr1x32x32(b, master->grp2));
    
    //  add external inputs to mix, output mix
//    if (in[2]) out[0] = add_fr1x32(mult_fr1x32x32(tmp, master->output), mult_fr1x32x32(in[2], master->direct));
    out[0] = mult_fr1x32x32(tmp, master->output);
    
//    if (in[3]) out[1] = add_fr1x32(mult_fr1x32x32(tmp, master->output), mult_fr1x32x32(in[3], master->direct));
    out[1] = mult_fr1x32x32(tmp, master->output);
    
    //  direct output
    out[2] = master->output3(master);
    out[3] = master->output4(master);
}

//process 0: off
fract32 pf_off(prgmTrack *t) {
    return 0;
}

//process 1: audio
fract32 pf_audio(prgmTrack *t) {
    return mult_fr1x32x32(t->envAmp.curve(&(t->envAmp)), t->output);
}

//process 2: amp
fract32 pf_amp(prgmTrack *t) {
    return t->envAmp.curve(&(t->envAmp));
}

//process 3: delay
fract32 pf_delay(prgmTrack *t) {
    env_tcd_set_loop(&(t->envAmp), t->uP);
    env_tcd_set_feedback(&(t->envAmp), t->sP);
    return t->envAmp.curve(&(t->envAmp));
}

//process 4: thru
fract32 pf_thru(prgmTrack *t) {
    return t->envAmp.curve(&(t->envAmp));
}

//process 5: tape
fract32 pf_tape(prgmTrack *t) {
    filter_1p_lo_in(&(t->pSlew), t->sP);

    if (filter_1p_sync(&(t->pSlew)))
    {
        env_tcd_set_decay(&(t->envAmp), t->sP);
        return t->envAmp.curve(&(t->envAmp));
    }
    else
    {
        env_tcd_set_decay(&(t->envAmp), filter_1p_lo_next(&(t->pSlew)));
        return t->envAmp.curve(&(t->envAmp));
    }
}

//process 6: cue
fract32 pf_cue(prgmTrack *t) {
    if (!t->c) env_tcd_set_trig(&(t->envAmp));
    
    filter_1p_lo_in(&(t->pSlew), t->sP);
    
    if (filter_1p_sync(&(t->pSlew)))
    {
        env_tcd_set_decay(&(t->envAmp), t->sP);
        return t->envAmp.curve(&(t->envAmp));
    }
    else
    {
        env_tcd_set_decay(&(t->envAmp), filter_1p_lo_next(&(t->pSlew)));
        return t->envAmp.curve(&(t->envAmp));
    }
}

//process 7: mute
fract32 pf_mute(prgmTrack *t) {
    fract32 tmp;
    
    if (t->flag == 0) tmp = 0;
    else if (t->flag == 1)
    {
        tmp = t->envAmp.curve(&(t->envAmp));
        tmp = 0;
    }
    else tmp = t->envAmp.curve(&(t->envAmp));
    
    if (t->mute)
    {
        t->hold = tmp;
        t->mute = 0;
        t->process = get_processptr(8);
        return tmp;
    }
    else return 0x00000000;
}

//process 8: while muted
fract32 pf_holdmute(prgmTrack *t) {
    fract32 tmp;
    
    if (t->flag == 0) ;
    else tmp = t->envAmp.curve(&(t->envAmp));

    return t->hold;
}

//process 9: unmute
fract32 pf_unmute(prgmTrack *t) {
    fract32 tmp, lo, hi;
    
    if (t->flag == 0) tmp = 0;
    else if (t->flag == 1)
    {
        tmp = t->envAmp.curve(&(t->envAmp));
        tmp = 0;
    }
    else tmp = t->envAmp.curve(&(t->envAmp));
    
    lo = sub_fr1x32(t->hold, 0xf);
    hi = add_fr1x32(t->hold, 0xf);
    
    if ((tmp >= lo || tmp <= hi) && t->mute)
    {
        t->mute = 0;
        t->process = get_processptr(t->flag);
        return tmp;
    }
    else return 0x00000000;
}

//process 10: TEST CLK MODE
fract32 pf_clock(prgmTrack *t) {
    substep += 1;
    
    if (substep > step - 1)
    {
        u8 n;
        substep = 0;
        
        for (n = 0; n < N_TRACKS; n++)
        {
            u16 c;
            c = track[n]->c += 1;
            if (c > track[n]->len - 1) c = track[n]->c = 0;
            
            if (sq[n]->tg[c])
            {
                track_set_output(track[n], sq[n]->l[c]);
                env_tcd_set_loop(&(track[n]->envAmp), sq[n]->b[c]);
                env_tcd_set_start(&(track[n]->envAmp), sq[n]->a[c]);
                env_tcd_set_trig(&(track[n]->envAmp));
            }
        }
    }
    else ;
 
    return 0;
}

//mix routing
static void route_mix(s32 t, s32 r) {
    //  mix
    if (r == 0)
    {
        track[t]->to_mix = 1;
        track[t]->to_grp1 = 0;
        track[t]->to_grp2 = 0;
    }
    //  group 1
    else if (r == 1)
    {
        track[t]->to_mix = 0;
        track[t]->to_grp1 = 1;
        track[t]->to_grp2 = 0;
    }
    //  group 1
    else if (r == 2)
    {
        track[t]->to_mix = 0;
        track[t]->to_grp1 = 0;
        track[t]->to_grp2 = 1;
    }
    //  group 1 + 2
    else if (r == 3)
    {
        track[t]->to_mix = 0;
        track[t]->to_grp1 = 1;
        track[t]->to_grp2 = 1;
    }
    //  mix + group 1
    else if (r == 4)
    {
        track[t]->to_mix = 1;
        track[t]->to_grp1 = 1;
        track[t]->to_grp2 = 0;
    }
    // mix + group 2
    else if (r == 5)
    {
        track[t]->to_mix = 1;
        track[t]->to_grp1 = 0;
        track[t]->to_grp2 = 1;
    }
    else ;
}

//parameter routing
void track_set_output(prgmTrack *t, fract32 out) { //rename to route_param
    //Parameter LINK, add here!
    //-> volume
    //-> pitch
    //-> decay
    //ROUTING algorithm, for parameter and for mix group settings, bitfield?
    
    t->output = out; //volume
    env_tcd_set_decay(&(t->envAmp), out); //pitch
    //decay
}

//direct outs
fract32 (*get_outputptr(u8 n))() {
    static fract32 (*outputs[])() =
    {
        out_off,
        out_vout0,
        out_vout1,
        out_vout2,
        out_vout3,
        out_vout4,
        out_vout5,
        out_vout6,
        out_vout7,
        out_aux,
    };
    
    return (n < 1 || n > N_DIROUTS) ? *outputs[0] : *outputs[n];
}

fract32 out_off(void) {
    return 0;
}

fract32 out_vout0(void) {
    return vout[0];
}

fract32 out_vout1(void) {
    return vout[1];
}

fract32 out_vout2(void) {
    return vout[2];
}

fract32 out_vout3(void) {
    return vout[3];
}

fract32 out_vout4(void) {
    return vout[4];
}

fract32 out_vout5(void) {
    return vout[5];
}

fract32 out_vout6(void) {
    return vout[6];
}

fract32 out_vout7(void) {
    return vout[7];
}

fract32 out_aux(void) {
    return aux;
}

void set_output3(u8 n) {
    master->output3 = get_outputptr(n);
}

void set_output4(u8 n) {
    master->output4 = get_outputptr(n);
}

//trig events
void module_set_clock_in(void) {
    u8 n;
    u16 c;

    for (n = 0; n < N_TRACKS; n++)
    {
        c = track[n]->c += 1;
        if (c > track[n]->len - 1) c = track[n]->c = 0;

        if (sq[n]->tg[c])
        {
            track_set_output(track[n], sq[n]->l[c]);
            env_tcd_set_loop(&(track[n]->envAmp), sq[n]->b[c]);
            env_tcd_set_start(&(track[n]->envAmp), sq[n]->a[c]);
            env_tcd_set_trig(&(track[n]->envAmp));
        }
    }
}

//RENAME TO SET_ALT_TRIG
void module_set_gate_out(void) {
    u8 n;
    
    for (n = 0; n < N_TRACKS; n++)
    {
        u16 c = track[n]->c;
        if (sq[n]->tg[c])
        {
            env_tcd_set_trig(&(track[n]->envAmp));
        }
    }
}


//  set parameters
void module_set_param(u32 idx, ParamValue v) {};

void param_set_sampleoffset(ParamValue v) {
    offset = v;
}

void param_set_samplevalue(ParamValue v) {
    data->sampleBuffer[offset] = v;
}

void param_reset_counter(void) {
    u8 n;
    
    substep = step - 1;

    for (n = 0; n < N_TRACKS; n++)
    {
        track[n]->c = track[n]->len - 1;
    }
}
/*
    for (n = 0; n < N_TRACKS; n++)
    {
        if (v > track[n]->len - 1)
        {
            c = 0;
        }
        else
        {
            c = v;
        }
        track[n]->c = c;
    }
*/

void module_set_sqparam(u32 s, u32 idx, ParamValue v) {

    switch(idx) {
        case eParamOffset:
            param_set_sampleoffset(v);
            break;
            
        case eParamSample:
            param_set_samplevalue(v);
            break;

        case eParamTrig0:
            env_tcd_set_trig(&(track[0]->envAmp));
            break;
        case eParamTrig1:
            env_tcd_set_trig(&(track[1]->envAmp));
            break;
        case eParamTrig2:
            env_tcd_set_trig(&(track[2]->envAmp));
            break;
        case eParamTrig3:
            env_tcd_set_trig(&(track[3]->envAmp));
            break;
        case eParamTrig4:
            env_tcd_set_trig(&(track[4]->envAmp));
            break;
        case eParamTrig5:
            env_tcd_set_trig(&(track[5]->envAmp));
            break;
        case eParamTrig6:
            env_tcd_set_trig(&(track[6]->envAmp));
            break;
        case eParamTrig7:
            env_tcd_set_trig(&(track[7]->envAmp));
            break;
            
        case eParamSetTrig0:
            sq[0]->tg[s] = v;
            break;
        case eParamSetTrig1:
            sq[1]->tg[s] = v;
            break;
        case eParamSetTrig2:
            sq[2]->tg[s] = v;
            break;
        case eParamSetTrig3:
            sq[3]->tg[s] = v;
            break;
        case eParamSetTrig4:
            sq[4]->tg[s] = v;
            break;
        case eParamSetTrig5:
            sq[5]->tg[s] = v;
            break;
        case eParamSetTrig6:
            sq[6]->tg[s] = v;
            break;
        case eParamSetTrig7:
            sq[7]->tg[s] = v;
            break;
            
        case eParamFlag0:
            track[0]->flag = v;
            track[0]->process = get_processptr(v);
            break;
        case eParamFlag1:
            track[1]->flag = v;
            track[1]->process = get_processptr(v);
            break;
        case eParamFlag2:
            track[2]->flag = v;
            track[2]->process = get_processptr(v);
            break;
        case eParamFlag3:
            track[3]->flag = v;
            track[3]->process = get_processptr(v);
            break;
        case eParamFlag4:
            track[4]->flag = v;
            track[4]->process = get_processptr(v);
            break;
        case eParamFlag5:
            track[5]->flag = v;
            track[5]->process = get_processptr(v);
            break;
        case eParamFlag6:
            track[6]->flag = v;
            track[6]->process = get_processptr(v);
            break;
        case eParamFlag7:
            track[7]->flag = v;
            track[7]->process = get_processptr(v);
            break;
            
        case eParamMuteFlag0:
            track[0]->mute = v;
            break;
        case eParamMuteFlag1:
            track[1]->mute = v;
            break;
        case eParamMuteFlag2:
            track[2]->mute = v;
            break;
        case eParamMuteFlag3:
            track[3]->mute = v;
            break;
        case eParamMuteFlag4:
            track[4]->mute = v;
            break;
        case eParamMuteFlag5:
            track[5]->mute = v;
            break;
        case eParamMuteFlag6:
            track[6]->mute = v;
            break;
        case eParamMuteFlag7:
            track[7]->mute = v;
            break;

        case eParamMute:
            track[v]->process = get_processptr(7);
            break;            
        case eParamUnMute:
            track[v]->process = get_processptr(9);
            break;
            
        case eParamCurve0:
            env_tcd_set_curve(&(track[0]->envAmp), v);
            break;
        case eParamCurve1:
            env_tcd_set_curve(&(track[1]->envAmp), v);
            break;
        case eParamCurve2:
            env_tcd_set_curve(&(track[2]->envAmp), v);
            break;
        case eParamCurve3:
            env_tcd_set_curve(&(track[3]->envAmp), v);
            break;
        case eParamCurve4:
            env_tcd_set_curve(&(track[4]->envAmp), v);
            break;
        case eParamCurve5:
            env_tcd_set_curve(&(track[5]->envAmp), v);
            break;
        case eParamCurve6:
            env_tcd_set_curve(&(track[6]->envAmp), v);
            break;
        case eParamCurve7:
            env_tcd_set_curve(&(track[7]->envAmp), v);
            break;

        case eParamLength:
            track[s]->len = v;
            break;
            
        case eParamInput0:
            env_tcd_set_input(&(track[0]->envAmp), v);
            break;
        case eParamInput1:
            env_tcd_set_input(&(track[1]->envAmp), v);
            break;
        case eParamInput2:
            env_tcd_set_input(&(track[2]->envAmp), v);
            break;
        case eParamInput3:
            env_tcd_set_input(&(track[3]->envAmp), v);
            break;
        case eParamInput4:
            env_tcd_set_input(&(track[4]->envAmp), v);
            break;
        case eParamInput5:
            env_tcd_set_input(&(track[5]->envAmp), v);
            break;
        case eParamInput6:
            env_tcd_set_input(&(track[6]->envAmp), v);
            break;
        case eParamInput7:
            env_tcd_set_input(&(track[7]->envAmp), v);
            break;
            
        case eParamInputLevel0:
            env_tcd_set_level(&(track[0]->envAmp), v);
            break;
        case eParamInputLevel1:
            env_tcd_set_level(&(track[1]->envAmp), v);
            break;
        case eParamInputLevel2:
            env_tcd_set_level(&(track[2]->envAmp), v);
            break;
        case eParamInputLevel3:
            env_tcd_set_level(&(track[3]->envAmp), v);
            break;
        case eParamInputLevel4:
            env_tcd_set_level(&(track[4]->envAmp), v);
            break;
        case eParamInputLevel5:
            env_tcd_set_level(&(track[5]->envAmp), v);
            break;
        case eParamInputLevel6:
            env_tcd_set_level(&(track[6]->envAmp), v);
            break;
        case eParamInputLevel7:
            env_tcd_set_level(&(track[7]->envAmp), v);
            break;
                        
        case eParamOffsetA0:
            env_tcd_set_start(&track[0]->envAmp, v);
            break;
        case eParamOffsetA1:
            env_tcd_set_start(&track[1]->envAmp, v);
            break;
        case eParamOffsetA2:
            env_tcd_set_start(&track[2]->envAmp, v);
            break;
        case eParamOffsetA3:
            env_tcd_set_start(&track[3]->envAmp, v);
            break;
        case eParamOffsetA4:
            env_tcd_set_start(&track[4]->envAmp, v);
            break;
        case eParamOffsetA5:
            env_tcd_set_start(&track[5]->envAmp, v);
            break;
        case eParamOffsetA6:
            env_tcd_set_start(&track[6]->envAmp, v);
            break;
        case eParamOffsetA7:
            env_tcd_set_start(&track[7]->envAmp, v);
            break;
            
        case eParamOffsetB0:
            env_tcd_set_loop(&track[0]->envAmp, v);
            break;
        case eParamOffsetB1:
            env_tcd_set_loop(&track[1]->envAmp, v);
            break;
        case eParamOffsetB2:
            env_tcd_set_loop(&track[2]->envAmp, v);
            break;
        case eParamOffsetB3:
            env_tcd_set_loop(&track[3]->envAmp, v);
            break;
        case eParamOffsetB4:
            env_tcd_set_loop(&track[4]->envAmp, v);
            break;
        case eParamOffsetB5:
            env_tcd_set_loop(&track[5]->envAmp, v);
            break;
        case eParamOffsetB6:
            env_tcd_set_loop(&track[6]->envAmp, v);
            break;
        case eParamOffsetB7:
            env_tcd_set_loop(&track[7]->envAmp, v);
            break;

        case eParamSampleOffsetA0:
            sq[0]->a[s] = v;
            break;
        case eParamSampleOffsetA1:
            sq[1]->a[s] = v;
            break;
        case eParamSampleOffsetA2:
            sq[2]->a[s] = v;
            break;
        case eParamSampleOffsetA3:
            sq[3]->a[s] = v;
            break;
        case eParamSampleOffsetA4:
            sq[4]->a[s] = v;
            break;
        case eParamSampleOffsetA5:
            sq[5]->a[s] = v;
            break;
        case eParamSampleOffsetA6:
            sq[6]->a[s] = v;
            break;
        case eParamSampleOffsetA7:
            sq[7]->a[s] = v;
            break;

        case eParamSampleOffsetB0:
            sq[0]->b[s] = v;
            break;
        case eParamSampleOffsetB1:
            sq[1]->b[s] = v;
            break;
        case eParamSampleOffsetB2:
            sq[2]->b[s] = v;
            break;
        case eParamSampleOffsetB3:
            sq[3]->b[s] = v;
            break;
        case eParamSampleOffsetB4:
            sq[4]->b[s] = v;
            break;
        case eParamSampleOffsetB5:
            sq[5]->b[s] = v;
            break;
        case eParamSampleOffsetB6:
            sq[6]->b[s] = v;
            break;
        case eParamSampleOffsetB7:
            sq[7]->b[s] = v;
            break;
            
        case eParamUnsigned0:
            track[0]->uP = v;
            break;
        case eParamUnsigned1:
            track[1]->uP = v;
            break;
        case eParamUnsigned2:
            track[2]->uP = v;
            break;
        case eParamUnsigned3:
            track[3]->uP = v;
            break;
        case eParamUnsigned4:
            track[4]->uP = v;
            break;
        case eParamUnsigned5:
            track[5]->uP = v;
            break;
        case eParamUnsigned6:
            track[6]->uP = v;
            break;
        case eParamUnsigned7:
            track[7]->uP = v;
            break;
            
        case eParamSigned0:
            track[0]->sP = v;
            break;
        case eParamSigned1:
            track[1]->sP = v;
            break;
        case eParamSigned2:
            track[2]->sP = v;
            break;
        case eParamSigned3:
            track[3]->sP = v;
            break;
        case eParamSigned4:
            track[4]->sP = v;
            break;
        case eParamSigned5:
            track[5]->sP = v;
            break;
        case eParamSigned6:
            track[6]->sP = v;
            break;
        case eParamSigned7:
            track[7]->sP = v;
            break;
            
        case eParamOutputLevel0:
            sq[0]->l[s] = v;
            break;
        case eParamOutputLevel1:
            sq[1]->l[s] = v;
            break;
        case eParamOutputLevel2:
            sq[2]->l[s] = v;
            break;
        case eParamOutputLevel3:
            sq[3]->l[s] = v;
            break;
        case eParamOutputLevel4:
            sq[4]->l[s] = v;
            break;
        case eParamOutputLevel5:
            sq[5]->l[s] = v;
            break;
        case eParamOutputLevel6:
            sq[6]->l[s] = v;
            break;
        case eParamOutputLevel7:
            sq[7]->l[s] = v;
            break;
            
        case eParamRouteMix:
            route_mix(s, v);
            break;
            
        case eParamMixLevel0:
            track[0]->mix = v;
            filter_1p_lo_in(&(track[0]->mSlew), v);
            break;
        case eParamMixLevel1:
            track[1]->mix = v;
            break;
        case eParamMixLevel2:
            track[2]->mix = v;
            break;
        case eParamMixLevel3:
            track[3]->mix = v;
            break;
        case eParamMixLevel4:
            track[4]->mix = v;
            break;
        case eParamMixLevel5:
            track[5]->mix = v;
            break;
        case eParamMixLevel6:
            track[6]->mix = v;
            break;
        case eParamMixLevel7:
            track[7]->mix = v;
            break;
            
        case eParamAuxLevel0:
            track[0]->aux = v;
            break;
        case eParamAuxLevel1:
            track[1]->aux = v;
            break;
        case eParamAuxLevel2:
            track[2]->aux = v;
            break;
        case eParamAuxLevel3:
            track[3]->aux = v;
            break;
        case eParamAuxLevel4:
            track[4]->aux = v;
            break;
        case eParamAuxLevel5:
            track[5]->aux = v;
            break;
        case eParamAuxLevel6:
            track[6]->aux = v;
            break;
        case eParamAuxLevel7:
            track[7]->aux = v;
            break;
                                  
        case eParamDirectOut3:
            set_output3(v);
            break;
        case eParamDirectOut4:
            set_output4(v);
            break;

        case eParamGroup1:
            master->grp1 = v;
            break;
        case eParamGroup2:
            master->grp2 = v;
            break;
        case eParamMaster:
            master->output = v;
            break;
            
        case eParamResetCounter:
            param_reset_counter();
            break;
        case eParamBPM:
            step = v;
            break;
            
        case eParamMotor0:
            env_tcd_set_motor(&track[0]->envAmp, v);
            break;
        case eParamMotor1:
            env_tcd_set_motor(&track[1]->envAmp, v);
            break;
        case eParamMotor2:
            env_tcd_set_motor(&track[2]->envAmp, v);
            break;
        case eParamMotor3:
            env_tcd_set_motor(&track[3]->envAmp, v);
            break;
        case eParamMotor4:
            env_tcd_set_motor(&track[4]->envAmp, v);
            break;
        case eParamMotor5:
            env_tcd_set_motor(&track[5]->envAmp, v);
            break;
        case eParamMotor6:
            env_tcd_set_motor(&track[6]->envAmp, v);
            break;
        case eParamMotor7:
            env_tcd_set_motor(&track[7]->envAmp, v);
            break;

        default:
            break;
    }
}

//add error here to stop make deploy scroll...