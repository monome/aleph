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

//audio buffer
sampleBuffer onchipbuffer[N_TRACKS];
u32 offset;

//sequencer
sqTrack *sq[N_TRACKS];
u16 counter;
u16 length;

//tracks
prgmTrack *track[N_TRACKS];

//cv outputs
static fract32 cvVal[N_TRACKS];
static u8 cvChn = 0;


//function declarations
//memory allocation
static PrgmTrackptr init_track(void);
static void init_track_parameters(prgmTrack *t);
static sqTrackptr init_sq(void);
static void init_sq_parameters(sqTrack *t);

//process frame
static void set_process(prgmTrack *t, u8 n);
static fract32 (*get_processptr(u8 n))();
static fract32 pf_off(prgmTrack *t);
static fract32 pf_audio(prgmTrack *t);
static fract32 pf_loop(prgmTrack *t);
static fract32 pf_aux_mixloop(prgmTrack *t);
static fract32 pf_aux_mix(prgmTrack *t);
static fract32 pf_aux_frq(prgmTrack *t);
static fract32 pf_cv(prgmTrack *t);
static fract32 pf_frq(prgmTrack *t);

//sequenced parameters
static void sqparam_set_trig(u8 t, u8 s, ParamValue v);
static void sqparam_set_flag(u8 t, u8 s, ParamValue v);
static void sqparam_set_curve(u8 t, u8 s, ParamValue v);
static void sqparam_set_time(u8 t, u8 s, ParamValue v);
static void sqparam_set_pos(u8 t, u8 s, ParamValue v);
static void sqparam_set_loop(u8 t, u8 s, ParamValue v);

//global parameters
static void param_set_level(prgmTrack *t, ParamValue v);
static void param_set_inputA(prgmTrack *t, ParamValue v);
static void param_set_inputB(prgmTrack *t, ParamValue v);
static void param_set_mix(prgmTrack *t, ParamValue v);
static void param_set_counter(ParamValue v);
static void param_set_length(ParamValue v);
static void sqparam_set_frq(u8 t, u8 s, ParamValue v);

//events
static void play_step(u8 c);


//function definitions
PrgmTrackptr init_track(void) {
    return(PrgmTrackptr)malloc(sizeof(prgmTrack));
}


void init_track_parameters(prgmTrack *t) {
    t->flag = 0;
    t->process = get_processptr(0);

    t->pF = 0;
    t->pM = 0;
    t->pL = 0;
    t->pP = 0;
    t->pLP = 0;
    filter_1p_lo_init(&(t->pSlew), 0xf);
    filter_1p_lo_set_slew(&(t->pSlew), 0x7cffffff); //TEST!!!
    
    env_tcd_init(&(t->envAmp));
}


sqTrackptr init_sq(void) {
    return(sqTrackptr)malloc(sizeof(sqTrack));
}


void init_sq_parameters(sqTrack *t) {
    u8 i;
    for (i=0; i<SQ_LEN; i++)
    {
        t->sqtg[i] = 0;
        t->sqf[i] = 0;
        t->sqc[i] = 0;
        t->sqt[i] = 0;
        t->sqp[i] = 0;
        t->sqfq[i] = 0;
    }
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
    
    //  init tracks
    for(n=0; n<N_TRACKS; n++)
    {
        track[n] = init_track();
        init_track_parameters(track[n]);
        sq[n] = init_sq();
        init_sq_parameters(sq[n]);
    }
    
    //  init buffer
    for(n=0; n<N_TRACKS; n++) {
        buffer_init(&(onchipbuffer[n]), data->sampleBuffer, BUFFER_SIZE);
    }
    
    //  init track playheads
    for(n=0; n<N_TRACKS; n++)
    {
        for(i=0; i<N_HEADS; i++)
        {
            buffer_head_init(&(track[n])->envAmp.head[i], &(onchipbuffer[n]));
        }
    }
    
    //  zero buffer
    for(i=0; i<BUFFER_SIZE; i++) data->sampleBuffer[i] = 0x00000000;
    
    //  init static variables
    counter = 0;
    length = SQ_LEN;
//    offset = 0;
    
    //  init parameters
    for(i=0; i<module_get_num_params(); i++) param_setup(i, 0);
}


void module_deinit(void) {}


extern u32 module_get_num_params(void) {
    return eParamNumParams;
}


void module_load_sample(s32 sample) {
//    u32 tmp = offset + idx;
//    data->sampleBuffer[offset] = sample;
}


//process frame
fract32 (*get_processptr(u8 n))() {
    static fract32 (*process[])() =
    {
        pf_off,                         //0
        pf_audio,                       //1
        pf_loop,                        //2
        pf_aux_mixloop,                 //3
        pf_aux_mix,                     //4
        pf_aux_frq,                     //5
        pf_cv,                          //6
        pf_frq,                         //7
    };

    return (n < 1 || n > 8) ? *process[0] : *process[n];
}


void set_process(prgmTrack *t, u8 n) {
    t->flag = n;
    t->process = get_processptr(n);
}


void module_process_frame(void) {
    fract32 tmp;

    //  mix aux's
//    tmp = 0;
//    tmp = add_fr1x32(tmp, mult_fr1x32x32(out[0], track[0]->pL));
    //TEST THIS!!!
    tmp = mult_fr1x32x32(out[0], track[0]->pL);
    tmp = add_fr1x32(tmp, mult_fr1x32x32(out[1], track[1]->pL));
    tmp = add_fr1x32(tmp, mult_fr1x32x32(out[2], track[2]->pL));
    tmp = add_fr1x32(tmp, mult_fr1x32x32(out[3], track[3]->pL));
    aux = tmp;
    
    for(cvChn=0;cvChn<N_TRACKS;cvChn++)
    {
        //  calculate audio outputs
        if(track[cvChn]->flag < 6)
        {
            out[cvChn] = 0x00000000;
            out[cvChn] = track[cvChn]->process(track[cvChn]);
        }
        else
        {
            //  calculate cv outputs
            cvVal[cvChn] = 0;
            cvVal[cvChn] = track[cvChn]->process(track[cvChn]);
            cv_update(cvChn, cvVal[cvChn]);
        }
    }
}


//process 0: off | audio
fract32 pf_off(prgmTrack *t) {
    return 0;
}


//process 1: none | audio
fract32 pf_audio(prgmTrack *t) {
    if(t->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return t->envAmp.curve(&(t->envAmp));
    }
}


//process 2: loop | audio
fract32 pf_loop(prgmTrack *t) {
    env_tcd_set_loop(&(t->envAmp), t->pLP);
    if(t->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return t->envAmp.curve(&(t->envAmp));
    }
}


//process 3: mix+loop | aux
fract32 pf_aux_mixloop(prgmTrack *t) {
    env_tcd_set_mix(&(t->envAmp), t->pM);
    env_tcd_set_loop(&(t->envAmp), t->pLP);
    if(t->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return t->envAmp.curve(&(t->envAmp));
    }
}


//process 4: mix | aux
fract32 pf_aux_mix(prgmTrack *t) {
    env_tcd_set_mix(&(t->envAmp), t->pM);
    if(t->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return t->envAmp.curve(&(t->envAmp));
    }
}


//process 5: frequency | aux
fract32 pf_aux_frq(prgmTrack *t) {
    filter_1p_lo_in(&(t->pSlew), t->pF);
    
    if(filter_1p_sync(&(t->pSlew)))
    {
        //output curve
//        bandlimit_set_f(&(t->envAmp.bpf), t->pF);
        return t->envAmp.curve(&(t->envAmp));
    }    
    else
    {
        //output encoder
//        bandlimit_set_f(&(t->envAmp.bpf), filter_1p_lo_next(&(t->pSlew)));
        return t->envAmp.curve(&(t->envAmp));
    }
}


//process 6: none | cv
fract32 pf_cv(prgmTrack *t) {
    if(t->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return t->envAmp.curve(&(t->envAmp));
    }
}


//process 7: frequency | cv
fract32 pf_frq(prgmTrack *t) {
    filter_1p_lo_in(&(t->pSlew), t->pF);
    return filter_1p_lo_next(&(t->pSlew));
}


//trig events
void module_set_trig(void) {
    u8 n, c;
    
    //  move counter
    counter++;
    if (counter > length - 1) c = counter = 0;
    c = counter;
    
//  update playhead position to AVR on first step
//    if (!i) gModuleData->headPosition = i;

    //  set parameters
    for (n = 0; n < N_TRACKS; n++)
    {
        if (sq[n]->sqtg[c])
        {
            set_process(track[n], sq[n]->sqf[c]);
            env_tcd_set_curve(&(track[n]->envAmp), sq[n]->sqc[c]);
            //  loop point before start position?!? or the other way around?!?
            env_tcd_set_loop(&(track[n]->envAmp), sq[n]->sqlp[c]);
            env_tcd_set_offset(&(track[n]->envAmp), sq[n]->sqp[c]);
            env_tcd_set_time(&(track[n]->envAmp), sq[n]->sqt[c]);
        }
    }
    
    //  trig curves
    env_tcd_set_trig(&(track[0]->envAmp), sq[0]->sqtg[c]);
    env_tcd_set_trig(&(track[1]->envAmp), sq[1]->sqtg[c]);
    env_tcd_set_trig(&(track[2]->envAmp), sq[2]->sqtg[c]);
    env_tcd_set_trig(&(track[3]->envAmp), sq[3]->sqtg[c]);
}


void module_set_reversetrig(void) {}


void play_step(u8 c) {
    u8 n;
    
    //  set parameters
    for (n = 0; n < N_TRACKS; n++)
    {
        if (sq[n]->sqtg[c])
        {
            set_process(track[n], sq[n]->sqf[c]);
            env_tcd_set_curve(&(track[n]->envAmp), sq[n]->sqc[c]);
            //  loop point must be set before start position
            env_tcd_set_loop(&(track[n]->envAmp), sq[n]->sqlp[c]);
            env_tcd_set_offset(&(track[n]->envAmp), sq[n]->sqp[c]);
            env_tcd_set_time(&(track[n]->envAmp), sq[n]->sqt[c]);
        }
    }
    
    //  trig curves
    env_tcd_set_trig(&(track[0]->envAmp), sq[0]->sqtg[c]);
    env_tcd_set_trig(&(track[1]->envAmp), sq[1]->sqtg[c]);
    env_tcd_set_trig(&(track[2]->envAmp), sq[2]->sqtg[c]);
    env_tcd_set_trig(&(track[3]->envAmp), sq[3]->sqtg[c]);
}


//  sequenced parameters
void sqparam_set_trig(u8 t, u8 s, ParamValue v) {
    sq[t]->sqtg[s] = v;
}

void sqparam_set_flag(u8 t, u8 s, ParamValue v) {
    sq[t]->sqf[s] = v;
}

void sqparam_set_curve(u8 t, u8 s, ParamValue v) {
    sq[t]->sqc[s] = v;
}

void sqparam_set_time(u8 t, u8 s, ParamValue v) {
    sq[t]->sqt[s] = v;
}

void sqparam_set_frq(u8 t, u8 s, ParamValue v) {
    //  set sequenced value
    sq[t]->sqfq[s] = v;
    //  allow process frame re-calculation
    track[t]->pF = v;
}

void sqparam_set_pos(u8 t, u8 s, ParamValue v) {
    //  set sequenced value
    sq[t]->sqp[s] = v;
    //  allow process frame re-calculation
//    track[t]->pP = v;
}

void sqparam_set_loop(u8 t, u8 s, ParamValue v) {
    //  set sequenced value
    sq[t]->sqlp[s] = v;
    //  allow process frame re-calculation
    track[t]->pLP = v;
}

//  global parameters
void param_set_inputA(prgmTrack *t, ParamValue v) {
    env_tcd_set_inputA(&(t->envAmp), v);
}

void param_set_inputB(prgmTrack *t, ParamValue v) {
    env_tcd_set_inputB(&(t->envAmp), v);
}

void param_set_mix(prgmTrack *t, ParamValue v) {
    t->pM = v;
}

void param_set_level(prgmTrack *t, ParamValue v) {
    t->pL = v;
}

void param_set_offset(ParamValue v) {
    offset = v;
}

void param_set_sample(ParamValue v) {
    data->sampleBuffer[offset] = v;
}

void param_set_buffer_length(sampleBuffer *buf, ParamValue v) {}
//    buf->frames = v;
//}

void param_set_recording_buffer(ParamValue v) {
//    buffer = v;
}

void param_set_recording_state(ParamValue v) {
    u8 i; // = buffer;
    
    switch(v) {
            
        case OFF:
            //  set process frame to audio with position control
            set_process(track[i], 0);
            //  set curve to buffer play
            env_tcd_set_curve(&(track[i]->envAmp), 9);
            break;
            
        case ARM:
            if (!counter)
            {
                //  set process frame to audio with position control
                set_process(track[i], 0);
                //  set curve to buffer record
                env_tcd_set_curve(&(track[i]->envAmp), 10);
                //  set trig
                env_tcd_set_trig(&(track[i]->envAmp), 1);
                
            }
            break;
            
            //USE env->headState!!!
        case ON:
            //  set process frame to audio with position control
            set_process(track[i], 0);
            //  set curve to buffer record
            env_tcd_set_curve(&(track[i]->envAmp), 10);
            //  set trig
            env_tcd_set_trig(&(track[i]->envAmp), 1);
            break;
            
        default:
            break;
    }
}

void param_set_counter(ParamValue v) {
    play_step(v);
}

void param_set_length(ParamValue v) {
    length = v;
}

void param_frame_forward(ParamValue v) {
    u8 n;
    for (n = 0; n < N_TRACKS; n++)
    {
        //change process
        //track[n]->envAmp->countTime += v;
        //get fract32 from curves with counttime as input
        //update process frame
    }
}

void param_frame_backward(ParamValue v) {
    u8 n;
    for (n = 0; n < N_TRACKS; n++)
    {
        //        track[n]->envAmp->countTime -= v;
    }
}

void param_set_motor_state(ParamValue v) {
//    motor = v;
}


void module_set_param(u32 idx, ParamValue v) {};


void module_set_sqparam(u32 s, u32 idx, ParamValue v) {

    switch(idx) {
        //  sequenced parameters
        case eParamTrig0:
            sqparam_set_trig(0, s, v);
            break;
        case eParamTrig1:
            sqparam_set_trig(1, s, v);
            break;
        case eParamTrig2:
            sqparam_set_trig(2, s, v);
            break;
        case eParamTrig3:
            sqparam_set_trig(3, s, v);
            break;
            
        case eParamFlag0:
            sqparam_set_flag(0, s, v);
            break;
        case eParamFlag1:
            sqparam_set_flag(1, s, v);
            break;
        case eParamFlag2:
            sqparam_set_flag(2, s, v);
            break;
        case eParamFlag3:
            sqparam_set_flag(3, s, v);
            break;
            
        case eParamCurve0:
            sqparam_set_curve(0, s, v);
            break;
        case eParamCurve1:
            sqparam_set_curve(1, s, v);
            break;
        case eParamCurve2:
            sqparam_set_curve(2, s, v);
            break;
        case eParamCurve3:
            sqparam_set_curve(3, s, v);
            break;
            
        case eParamTime0:
            sqparam_set_time(0, s, v);
            break;
        case eParamTime1:
            sqparam_set_time(1, s, v);
            break;
        case eParamTime2:
            sqparam_set_time(2, s, v);
            break;
        case eParamTime3:
            sqparam_set_time(3, s, v);
            break;
            
        case eParamPosition0:
            sqparam_set_pos(0, s, v);
            break;
        case eParamPosition1:
            sqparam_set_pos(1, s, v);
            break;
        case eParamPosition2:
            sqparam_set_pos(2, s, v);
            break;
        case eParamPosition3:
            sqparam_set_pos(3, s, v);
            break;
            
        case eParamLoop0:
            sqparam_set_loop(0, s, v);
            break;
        case eParamLoop1:
            sqparam_set_loop(1, s, v);
            break;
        case eParamLoop2:
            sqparam_set_loop(2, s, v);
            break;
        case eParamLoop3:
            sqparam_set_loop(3, s, v);
            break;

        //  global parameters
        case eParamInputA0:
            param_set_inputA(track[0], v);
            break;
        case eParamInputA1:
            param_set_inputA(track[1], v);
            break;
        case eParamInputA2:
            param_set_inputA(track[2], v);
            break;
        case eParamInputA3:
            param_set_inputA(track[3], v);
            break;

        case eParamInputB0:
            param_set_inputB(track[0], v);
            break;
        case eParamInputB1:
            param_set_inputB(track[1], v);
            break;
        case eParamInputB2:
            param_set_inputB(track[2], v);
            break;
        case eParamInputB3:
            param_set_inputB(track[3], v);
            break;
            
        case eParamMix0:
            param_set_mix(track[0], v);
            break;
        case eParamMix1:
            param_set_mix(track[1], v);
            break;
        case eParamMix2:
            param_set_mix(track[2], v);
            break;
        case eParamMix3:
            param_set_mix(track[3], v);
            break;
            
        case eParamLevel0:
            param_set_level(track[0], v);
            break;
        case eParamLevel1:
            param_set_level(track[1], v);
            break;
        case eParamLevel2:
            param_set_level(track[2], v);
            break;
        case eParamLevel3:
            param_set_level(track[3], v);
            break;
            
        case eParamFrequency0:
            sqparam_set_frq(0, s, v);
            break;
        case eParamFrequency1:
            sqparam_set_frq(1, s, v);
            break;
        case eParamFrequency2:
            sqparam_set_frq(2, s, v);
            break;
        case eParamFrequency3:
            sqparam_set_frq(3, s, v);
            break;
            
        case eParamOffset:
            param_set_offset(v);
            break;

        case eParamSample:
            param_set_sample(v);
            break;

        case eParamBufferLength0:
//            param_set_buffer_length(&(inBuf[0]), v);
            break;
        case eParamBufferLength1:
//            param_set_buffer_length(&(inBuf[1]), v);
            break;
        case eParamBufferLength2:
//            param_set_buffer_length(&(inBuf[2]), v);
            break;
        case eParamBufferLength3:
//            param_set_buffer_length(&(inBuf[3]), v);
            break;
            
        case eParamToggleBuf:
//            param_set_recording_buffer(v);
            break;
        case eParamToggleRec:
//            param_set_recording_state(v);
            break;
            
        case eParamSqCounter:
            param_set_counter(v);
            break;

        case eParamSqLength:
            param_set_length(v);
            break;
            
        case eParamFrameFwd:
//            param_frame_forward(v);
            break;
            
        case eParamFrameBwd:
//            param_frame_backward(v);
            break;
            
        case eParamToggleMotor:
//            param_set_motor_state(v);
            break;
            
        default:
            break;
    }
}
