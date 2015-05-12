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
static void set_process(prgmTrack *t, u8 n);
static fract32 (*get_processptr(u8 n))();
static fract32 pf_off(prgmTrack *t);
static fract32 pf_audio(prgmTrack *t);
static fract32 pf_amp(prgmTrack *t);
static fract32 pf_delay(prgmTrack *t);
static fract32 pf_thru(prgmTrack *t);
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
static fract32 out_aux0(void);
static fract32 out_aux1(void);
static fract32 (*get_outputptr(u8 n))();


//events
static void param_set_counter(ParamValue v);


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
    t->uP = 512;
    t->sP = 0;
    
    t->mix = 0;
    t->panL = 0;
    t->panR = 0;
    t->aux1 = 0;
    t->aux2 = 0;

    t->len = SQ_LEN;
    t->c = 0;
    
    filter_1p_lo_init(&(t->pSlew), 0xf);
    filter_1p_lo_set_slew(&(t->pSlew), 0x3fffffff); //TEST!!!
    
    env_tcd_init(&(t->envAmp));
}


PrgmMasterptr init_master(void) {
    return(PrgmMasterptr)malloc(sizeof(prgmMaster));
}

void init_master_parameters() {
    master->output3 = get_outputptr(0);
    master->output4 = get_outputptr(0);
    
    master->output = FR32_MAX;
        
    master->aux1panL = 0;
    master->aux1panR = sub_fr1x32(FR32_MAX, 0);

    master->aux2panL = 0;
    master->aux2panR = sub_fr1x32(FR32_MAX, 0);
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
    
    //  init buffer
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
    };

    return (n < 1 || n > 5) ? *process[0] : *process[n];
}


void set_process(prgmTrack *t, u8 n) {
    t->flag = n;
    t->process = get_processptr(n);
}


void module_process_frame(void) {
    u8 i;
    fract32 tmp;
    
    //  mix aux 1
    tmp = 0x00000000;
    
    for(i=0;i<N_TRACKS;i++)
    {
        if (track[i]->aux1)
            tmp = add_fr1x32(tmp, mult_fr1x32x32(vout[i], track[i]->aux1));
        else ;
    }    
    aux[0] = tmp;
    
    //  mix aux 2
    tmp = 0x00000000;
    
    for(i=0;i<N_TRACKS;i++)
    {
        if (track[i]->aux2)
            tmp = add_fr1x32(tmp, mult_fr1x32x32(vout[i], track[i]->aux2));
        else ;
    }    
    aux[1] = tmp;

    //  process audio
    for(i=0;i<N_TRACKS;i++)
    {
        //  skip inactive|muted tracks
        if(!track[i]->flag)
        {
            vout[i] = 0x00000000;
        }
        else
        {
            //  calculate active tracks
            vout[i] = 0x00000000;
            vout[i] = track[i]->process(track[i]);
        }
    }
    
    //  mix left output
    tmp = 0x00000000;
    
    for(i=0;i<N_TRACKS;i++)
    {
        tmp = add_fr1x32(tmp, mult_fr1x32x32(vout[i], track[i]->mix));
    }
    out[0] = mult_fr1x32x32(tmp, master->output);
    
    //  mix right output
    tmp = 0x00000000;

    for(i=0;i<N_TRACKS;i++)
    {
        tmp = add_fr1x32(tmp, mult_fr1x32x32(vout[i], track[i]->mix));
    }
    out[1] = mult_fr1x32x32(tmp, master->output);

    //  route direct outputs
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

void track_set_output(prgmTrack *t, fract32 out) {
    t->output = out;
    env_tcd_set_decay(&(t->envAmp), out);
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
        out_aux0,
        out_aux1,
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

fract32 out_aux0(void) {
    return aux[0];
}

fract32 out_aux1(void) {
    return aux[1];
}

void set_output3(u8 n) {
    master->output3 = get_outputptr(n);
}

void set_output4(u8 n) {
    master->output4 = get_outputptr(n);
}


//events
void module_set_event(void) {
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


//  set parameters
void module_set_param(u32 idx, ParamValue v) {};

void param_set_sampleoffset(ParamValue v) {
    offset = v;
}

void param_set_samplevalue(ParamValue v) {
    data->sampleBuffer[offset] = v;
}

void param_set_counter(ParamValue v) {
    u8 n;
    u16 c;
    
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
}

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
            set_process(track[0], v);
            break;
        case eParamFlag1:
            set_process(track[1], v);
            break;
        case eParamFlag2:
            set_process(track[2], v);
            break;
        case eParamFlag3:
            set_process(track[3], v);
            break;
        case eParamFlag4:
            set_process(track[4], v);
            break;
        case eParamFlag5:
            set_process(track[5], v);
            break;
        case eParamFlag6:
            set_process(track[6], v);
            break;
        case eParamFlag7:
            set_process(track[7], v);
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
            
        case eParamLength0:
            track[0]->len = v;
            break;
        case eParamLength1:
            track[1]->len = v;
            break;
        case eParamLength2:
            track[2]->len = v;
            break;
        case eParamLength3:
            track[3]->len = v;
            break;
        case eParamLength4:
            track[4]->len = v;
            break;
        case eParamLength5:
            track[5]->len = v;
            break;
        case eParamLength6:
            track[6]->len = v;
            break;
        case eParamLength7:
            track[7]->len = v;
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
            
        case eParamMixLevel0:
            track[0]->mix = v;
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
            
        case eParamMixPanL0:
            track[0]->panL = v;
            break;
        case eParamMixPanL1:
            track[1]->panL = v;
            break;
        case eParamMixPanL2:
            track[2]->panL = v;
            break;
        case eParamMixPanL3:
            track[3]->panL = v;
            break;
        case eParamMixPanL4:
            track[4]->panL = v;
            break;
        case eParamMixPanL5:
            track[5]->panL = v;
            break;
        case eParamMixPanL6:
            track[6]->panL = v;
            break;
        case eParamMixPanL7:
            track[7]->panL = v;
            break;
            
        case eParamMixPanR0:
            track[0]->panR = v;
            break;
        case eParamMixPanR1:
            track[1]->panR = v;
            break;
        case eParamMixPanR2:
            track[2]->panR = v;
            break;
        case eParamMixPanR3:
            track[3]->panR = v;
            break;
        case eParamMixPanR4:
            track[4]->panR = v;
            break;
        case eParamMixPanR5:
            track[5]->panR = v;
            break;
        case eParamMixPanR6:
            track[6]->panR = v;
            break;
        case eParamMixPanR7:
            track[7]->panR = v;
            break;
            
        case eParamAux1Level0:
            track[0]->aux1 = v;
            break;
        case eParamAux1Level1:
            track[1]->aux1 = v;
            break;
        case eParamAux1Level2:
            track[2]->aux1 = v;
            break;
        case eParamAux1Level3:
            track[3]->aux1 = v;
            break;
        case eParamAux1Level4:
            track[4]->aux1 = v;
            break;
        case eParamAux1Level5:
            track[5]->aux1 = v;
            break;
        case eParamAux1Level6:
            track[6]->aux1 = v;
            break;
        case eParamAux1Level7:
            track[7]->aux1 = v;
            break;
            
        case eParamAux2Level0:
            track[0]->aux2 = v;
            break;
        case eParamAux2Level1:
            track[1]->aux2 = v;
            break;
        case eParamAux2Level2:
            track[2]->aux2 = v;
            break;
        case eParamAux2Level3:
            track[3]->aux2 = v;
            break;
        case eParamAux2Level4:
            track[4]->aux2 = v;
            break;
        case eParamAux2Level5:
            track[5]->aux2 = v;
            break;
        case eParamAux2Level6:
            track[6]->aux2 = v;
            break;
        case eParamAux2Level7:
            track[7]->aux2 = v;
            break;
            
        case eParamAux1PanL:
            master->aux1panL = v;
            break;
        case eParamAux1PanR:
            master->aux1panR = v;
            break;
        case eParamAux2PanL:
            master->aux2panL = v;
            break;
        case eParamAux2PanR:
            master->aux2panR = v;
            break;
            
        case eParamDirectOut3:
            set_output3(v);
            break;
        case eParamDirectOut4:
            set_output4(v);
            break;
            
        case eParamMaster:
            master->output = v;
            break;
            
        case eParamCounter:
            param_set_counter(v);
            break;
                        
        default:
            break;
    }
}
