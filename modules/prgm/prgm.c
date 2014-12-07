//prgm.c
//aleph-bfin

#include "prgm.h"

//data types
typedef struct _prgmData {
    ModuleData super;
    ParamData mParamData[eParamNumParams];
//    volatile fract32 inputBuffer[N_TRACKS][INPUT_BUF_FRAMES];

    s32 sqf[N_TRACKS][SQ_LEN];
    s32 sqc[N_TRACKS][SQ_LEN];
    s32 sqt[N_TRACKS][SQ_LEN];
    s32 datatestval;
} prgmData;

ModuleData *gModuleData;

//pointer to SDRAM (all external memory)
prgmData *data;

//buffers
inputBuffer inBuf[N_TRACKS];

//sample buffer test
inputBuffer sampleBuf;

//counter
u8 counter;

//tracks
prgmTrack *track[N_TRACKS];

//patterns
//u32 sqpP[N_TRACKS][SQ_LEN];
static SQ base;
static SQ *pattern[N_TRACKS];

//cv outputs
static fract32 cvVal[N_TRACKS];
static u8 cvChn = 0;

//memory allocation
static PrgmTrackptr init_track(void);
static void init_track_parameters(prgmTrack *t);

//parameters
static void param_set_f(u8 t, ParamValue p);
static void param_set_c(u8 t, ParamValue p);
static void param_set_lvl(prgmTrack *t, ParamValue p);
static void param_set_frq(prgmTrack *t, ParamValue p);
static void param_set_x(prgmTrack *t, ParamValue p);

//process frame
static void set_process(prgmTrack *t, u8 n);
static fract32 (*get_processptr(u8 n))();
static fract32 p_Pos_audio(prgmTrack *t);
static fract32 p_audio(prgmTrack *t);
static fract32 p_off(prgmTrack *t);
static fract32 p_cv(prgmTrack *t);
static fract32 p_FrqSlw_cv(prgmTrack *t);

//function definitions
PrgmTrackptr init_track(void) {
    return(PrgmTrackptr)malloc(sizeof(prgmTrack));
}


void init_track_parameters(prgmTrack *t) {
    t->flag = 0;
    t->process = get_processptr(0);

    t->pL = 0;
    t->pP = 0;
    t->pF = 0;
    filter_1p_lo_init(&(t->pFSlew), 0xf);
    filter_1p_lo_set_slew(&(t->pFSlew), 0x77ffffff); //TEST!!!
    t->pX = 0;
    
    env_tcd_init(&(t->envAmp));
}


static SQptr init_sq(void) {
    return(SQptr)malloc(SQ_LEN * sizeof(SQ));
}


static inline void param_setup(u32 id, ParamValue v) {
    gModuleData->paramData[id].value = v;
    module_set_param(id, v);
}


void module_init(void) {
    u32 i, n;
    data = (prgmData*)SDRAM_ADDRESS;
    gModuleData = &(data->super);
    strcpy(gModuleData->name, "aleph-prgm");
    gModuleData->paramData = data->mParamData;
    gModuleData->numParams = eParamNumParams;
    
    
    for(n=0; n<N_TRACKS; n++)
    {
        track[n] = init_track();
        init_track_parameters(track[n]);

        pattern[n] = init_sq();
    }

        inputbuffer_init(&sampleBuf, gModuleData->sampleBuffer, gModuleData->sampleBufferSize);
        buffer_head_init(&(track[0])->envAmp.head, &sampleBuf);
    
//        buffer_init(&(inBuf[n]), data->inputBuffer[n], INPUT_BUF_FRAMES);
//        buffer_head_init(&(track[n])->envAmp.head, &(inBuf[n]));
//    }
    
    //  init buffers to zero
/*
    for (n=0; n<N_TRACKS; n++)
    {
        for (i=0; i<INPUT_BUF_FRAMES; i++) data->inputBuffer[n][i] = 0x00000000;
    }
*/
    for (n=0; n<N_TRACKS; n++) {
        for (i=0; i<SQ_LEN; i++)
        {
            data->sqf[n][i] = 0;
            data->sqc[n][i] = 0;
//            data->sqt[n][i] = 0;
        }
    }
    
    for (n=0; n<N_TRACKS; n++) {
        pattern[n] = &base;
        pattern[n] = pattern[n]->s.ptr;
        
        //zero
        pattern[n]->s.t = 0;
        pattern[n]->s.pP = 0;
        
        //assign
//        track[n]->envAmp.time = &(pattern[n]->s.t);
        track[n]->pP = &(pattern[n]->s.pP);
    }

    //  init data parameters to zero
    counter = 0;
    
    for(i=0; i<module_get_num_params(); i++) { param_setup(i, 0); }
}


void module_deinit(void) {}


extern u32 module_get_num_params(void) {
  return eParamNumParams;
}


//process frame
fract32 (*get_processptr(u8 n))() {
    static fract32 (*process[])() =
    {
        //  p || input parameters || custom dsp || output
        p_Pos_audio,                //0
        p_audio,                    //1
        p_off,                      //2
        p_cv,                       //3
        p_FrqSlw_cv,                //4
    };

    return (n < 1 || n > 5) ? *process[0] : *process[n];
}


void set_process(prgmTrack *t, u8 n) {
    t->flag = n;
    t->process = get_processptr(n);
}


void module_process_frame(void) {
    for(cvChn=0;cvChn<N_TRACKS;cvChn++)
    {
        if(track[cvChn]->flag < 2)
        {
            out[cvChn] = 0x00000000;
            out[cvChn] = track[cvChn]->process(track[cvChn]);
        }
        else
        {
            cvVal[cvChn] = 0;
            cvVal[cvChn] = track[cvChn]->process(track[cvChn]);
            cv_update(cvChn, cvVal[cvChn]);
        }
    }
}


//buffer Position || audio
fract32 p_Pos_audio(prgmTrack *t) {
    env_tcd_set_pos(&(t->envAmp), *(t->pP));
    if(t->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return t->envAmp.curve(&(t->envAmp));
    }
}


//audio
fract32 p_audio(prgmTrack *t) {
    if(t->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return t->envAmp.curve(&(t->envAmp));
    }
}


//off
fract32 p_off(prgmTrack *t) {
    return 0;
}


//cv
fract32 p_cv(prgmTrack *t) {
    if(t->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return t->envAmp.curve(&(t->envAmp));
//        return env_tcd_next(&(t->envAmp));
    }
}


//Frequency with Slew || cv
fract32 p_FrqSlw_cv(prgmTrack *t) {
    filter_1p_lo_in(&(t->pFSlew), t->pF);
    return filter_1p_lo_next(&(t->pFSlew));
}


static void sq_next(prgmTrack *t) {
    (*t->pP)++;
}


//trig events
//void param_set_trig(ParamValue p) {
void module_set_trig(void) {
    u8 i, c;
    c = counter;
    for (i = 0; i < N_TRACKS; i++)
    {
//        sq_next(track[i]);
        set_process(track[i], data->sqf[i][c]);
        env_tcd_set_curve(&(track[i]->envAmp), data->sqc[i][c]);
        env_tcd_set_time(&(track[i]->envAmp), data->sqt[i][c]);
//        env_tcd_set_time(&(track[i]->envAmp), *(track[i]->envAmp.time));
        env_tcd_set_pos(&(track[i]->envAmp), *(track[i]->pP));
    }
    
    env_tcd_set_trig(&(track[0]->envAmp), 1);
    env_tcd_set_trig(&(track[1]->envAmp), 1);
    env_tcd_set_trig(&(track[2]->envAmp), 1);
    env_tcd_set_trig(&(track[3]->envAmp), 1);
}
/*
 void param_set_trig(ParamValue p) {
 u8 i, c;
 c = counter;
 
 for (i=0; i<N_TRACKS; i++)
 {
 //if(trigs[i][c])
 //        {
 track[i]->pP = data->sqpP[i][c];
 set_process(track[i], data->sqf[i][c]);
 env_tcd_set_curve(&(track[i]->envAmp), data->sqc[i][c]);
 env_tcd_set_time(&(track[i]->envAmp), data->sqt[i][c]);
 //    }
 }
 
 //    env_tcd_set_pos(&(track[0]->envAmp), data->sqpP[0][c]);
 //    env_tcd_set_pos(&(track[0]->envAmp), swap_s32(data->datatestval));
 env_tcd_set_pos(&(track[0]->envAmp), swap_s32(data->datatestval));
 
 env_tcd_set_trig(&(track[0]->envAmp), (p >> 24) & 0xff);
 env_tcd_set_trig(&(track[1]->envAmp), (p >> 16) & 0xff);
 env_tcd_set_trig(&(track[2]->envAmp), (p >> 8) & 0xff);
 env_tcd_set_trig(&(track[3]->envAmp), p & 0xff);
 }
 */

//edit parameters
static void param_set_pP(prgmTrack *t, ParamValue v) {
    *(t->pP) = v;
}

/*
void param_set_t(prgmTrack *t , ParamValue v) {
    *(t->envAmp.time) = v;
}
*/
void param_set_t(u8 t, ParamValue p) {
    data->sqt[t][counter] = p;
}






void param_set_f(u8 t, ParamValue p) {
    data->sqf[t][counter] = p;
}


void param_set_c(u8 t, ParamValue p) {
    data->sqc[t][counter] = p;
}


void param_set_lvl(prgmTrack *t, ParamValue p) {
    t->pL = p;
}


void param_set_frq(prgmTrack *t, ParamValue p) {
    t->pF = p;
}


void param_set_x(prgmTrack *t, ParamValue p) {
    t->pX = p;
}

/*
static void param_set_testval(ParamValue p) {
    data->testval = p;
}
*/

void module_set_param(u32 idx, ParamValue v) {
    switch(idx) {
        case eParamTrig:
            module_set_trig();
            break;

        case eParamFlag0:
            param_set_f(0, v);
            break;
        case eParamFlag1:
            param_set_f(1, v);
            break;
        case eParamFlag2:
            param_set_f(2, v);
            break;
        case eParamFlag3:
            param_set_f(3, v);
            break;

        case eParamCurve0:
            param_set_c(0, v);
            break;
        case eParamCurve1:
            param_set_c(1, v);
            break;
        case eParamCurve2:
            param_set_c(2, v);
            break;
        case eParamCurve3:
            param_set_c(3, v);
            break;
            
        case eParamTime0:
            param_set_t(0, v);
//            param_set_t(track[0], v);
            break;
        case eParamTime1:
            param_set_t(0, v);
//            param_set_t(track[1], v);
            break;
        case eParamTime2:
            param_set_t(0, v);
//            param_set_t(track[2], v);
            break;
        case eParamTime3:
            param_set_t(0, v);
//            param_set_t(track[3], v);
            break;
            
        case eParamL0:
            param_set_lvl(track[0], v);
            break;
        case eParamL1:
            param_set_lvl(track[1], v);
            break;
        case eParamL2:
            param_set_lvl(track[2], v);
            break;
        case eParamL3:
            param_set_lvl(track[3], v);
            break;

        case eParamP0:
            param_set_pP(track[0], v);
            break;
        case eParamP1:
            param_set_pP(track[1], v);
            break;
        case eParamP2:
            param_set_pP(track[2], v);
            break;
        case eParamP3:
            param_set_pP(track[3], v);
            break;
            
        case eParamF0:
            param_set_frq(track[0], v);
            break;
        case eParamF1:
            param_set_frq(track[1], v);
            break;
        case eParamF2:
            param_set_frq(track[2], v);
            break;
        case eParamF3:
            param_set_frq(track[3], v);
            break;
            
        case eParamX0:
            param_set_x(track[0], v);
            break;
        case eParamX1:
            param_set_x(track[1], v);
            break;
        case eParamX2:
            param_set_x(track[2], v);
            break;
        case eParamX3:
            param_set_x(track[3], v);
            break;
            
        case eParamTest:
//            param_set_testval(track[0], v);
            break;
            
        case eParamCounter:
//            param_set_counter(v);
            break;
            
        default:
            break;
    }
}
