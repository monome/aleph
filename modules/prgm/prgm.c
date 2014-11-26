//prgm.c
//aleph-bfin

#include "prgm.h"

//data types
typedef struct _prgmData {
    ModuleData super;
    ParamData mParamData[eParamNumParams];
    volatile fract32 inputBuffer[N_TRACKS][INPUT_BUF_FRAMES];
} prgmData;

ModuleData *gModuleData;

//pointer to SDRAM (all external memory)
prgmData *data;

//buffers
inputBuffer inBuf[N_TRACKS];

//tracks
prgmTrack *track[N_TRACKS];

//cv outputs
static fract32 cvVal[N_TRACKS];
static u8 cvChn = 0;

//memory allocation
static PrgmTrackptr init_track(void);
static void init_track_parameters(prgmTrack *t);

//parameters
static void param_set_lvl(prgmTrack *t, ParamValue p);
static void param_set_pos(prgmTrack *t, ParamValue p);
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


static inline void param_setup(u32 id, ParamValue v) {
    gModuleData->paramData[id].value = v;
    module_set_param(id, v);
}


void module_init(void) {
    u32 i;
    data = (prgmData*)SDRAM_ADDRESS;
    gModuleData = &(data->super);
    strcpy(gModuleData->name, "aleph-prgm");
    gModuleData->paramData = data->mParamData;
    gModuleData->numParams = eParamNumParams;
    
    for(i=0; i<N_TRACKS; i++)
    {
        track[i] = init_track();
        init_track_parameters(track[i]);
        buffer_init(&(inBuf[i]), data->inputBuffer[i], INPUT_BUF_FRAMES);
        buffer_head_init(&(track[i])->envAmp.head, &(inBuf[i]));
    }
    
    for(i=0; i<module_get_num_params(); i++) { param_setup(i, 0); }
}


void module_deinit(void) {}


extern u32 module_get_num_params(void) {
  return eParamNumParams;
}


//process frame
//p || input parameters || custom dsp || output
static fract32 (*process[])() =
{
    p_Pos_audio,                //0
    p_audio,                    //1
    p_off,                      //2
    p_cv,                       //3
    p_FrqSlw_cv,                //4
};


fract32 (*get_processptr(u8 n))() {
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
//            cvVal[cvChn] = frame_next(track[cvChn]);
            cvVal[cvChn] = track[cvChn]->process(track[cvChn]);
            cv_update(cvChn, cvVal[cvChn]);
        }
    }
}


//head Position || audio
fract32 p_Pos_audio(prgmTrack *t) {
    env_tcd_set_pos(&(t->envAmp), t->pP);
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


//parameters
void param_set_lvl(prgmTrack *t, ParamValue p) {
    t->pL = p;
}

void param_set_pos(prgmTrack *t, ParamValue p) {
    t->pP = p;
}


void param_set_frq(prgmTrack *t, ParamValue p) {
    t->pF = p;
}


void param_set_x(prgmTrack *t, ParamValue p) {
    t->pX = p;
}

void module_set_param(u32 idx, ParamValue v) {
    switch(idx) {
        case eParamTrig0:
            env_tcd_set_trig(&(track[0]->envAmp), v);
            break;
        case eParamTrig1:
            env_tcd_set_trig(&(track[1]->envAmp), v);
            break;
        case eParamTrig2:
            env_tcd_set_trig(&(track[2]->envAmp), v);
            break;
        case eParamTrig3:
            env_tcd_set_trig(&(track[3]->envAmp), v);
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
            
        case eParamTime0:
            env_tcd_set_time(&(track[0]->envAmp), v);
            break;
        case eParamTime1:
            env_tcd_set_time(&(track[1]->envAmp), v);
            break;
        case eParamTime2:
            env_tcd_set_time(&(track[2]->envAmp), v);
            break;
        case eParamTime3:
            env_tcd_set_time(&(track[3]->envAmp), v);
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
            param_set_pos(track[0], v);
            break;
        case eParamP1:
            param_set_pos(track[1], v);
            break;
        case eParamP2:
            param_set_pos(track[2], v);
            break;
        case eParamP3:
            param_set_pos(track[3], v);
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
            
        default:
            break;
    }
}
