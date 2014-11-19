//prgm.c
//aleph-bfin

#include "prgm.h"

//data types
typedef struct _prgmData {
    ModuleData super;
    ParamData mParamData[eParamNumParams];
    volatile fract32 cvBuffer[N_CVOUTPUTS][PRGM_BUF_FRAMES];
} prgmData;

ModuleData *gModuleData;

//pointer to SDRAM (all external memory)
prgmData *data;

//buffers
cvBuffer prgmBuf[N_CVOUTPUTS];

//cv outputs
prgmCvChannel *cvchannel[N_CVOUTPUTS];
static fract32 cvVal[N_CVOUTPUTS]; //volatile?!
static u8 cvChn = 0;

static PrgmCvChannelpointer init_channel(void);
static void init_cv_parameters(prgmCvChannel *cvchannel);
static void cv_set_f(prgmCvChannel *cvchannel, ParamValue f);
static void cv_set_t(prgmCvChannel *cvchannel, ParamValue t);

//frame process
static void set_flag(prgmCvChannel *cvchannel, ParamValue flag);
static fract32 (*get_processptr(u8 n))();
static void set_process(prgmCvChannel *cvchannel, u8 n);
static fract32 frame_next(prgmCvChannel *cvchannel);
static fract32 process_cv(prgmCvChannel *cvchannel);
static fract32 process_audio(prgmCvChannel *cvchannel);
    

//static function definitions
PrgmCvChannelpointer init_channel(void) {
    return(PrgmCvChannelpointer)malloc(sizeof(prgmCvChannel));
}

void init_cv_parameters(prgmCvChannel *cv) {
    cv->process = 0;
    cv->flag = 0;

    cv->f = 0;
    cv->t = 0;
    filter_1p_lo_init(&(cv->fSlew), 0xf);
    //  encoder source response
    filter_1p_lo_set_slew(&(cv->fSlew), 0x77ffffff); //TEST!!!
    env_tcd_init(&(cv->envAmp));
}


void cv_set_f(prgmCvChannel *cvchannel, ParamValue f) {
    cvchannel->f = f;
}


void cv_set_t(prgmCvChannel *cvchannel, ParamValue t) {
    cvchannel->t = t;
}


void set_flag(prgmCvChannel *cvchannel, ParamValue flag) {
    cvchannel->flag = flag;
}


static inline void param_setup(u32 id, ParamValue v) {
    gModuleData->paramData[id].value = v;
    module_set_param(id, v);
}


//external functions
void module_init(void) {
    u32 i;
    data = (prgmData*)SDRAM_ADDRESS;
    gModuleData = &(data->super);
    strcpy(gModuleData->name, "aleph-prgm");
    gModuleData->paramData = data->mParamData;
    gModuleData->numParams = eParamNumParams;
    
    for(i=0; i<N_CVOUTPUTS; i++)
    {
        cvchannel[i] = init_channel();
        init_cv_parameters(cvchannel[i]);
        buffer_init(&(prgmBuf[i]), data->cvBuffer[i], PRGM_BUF_FRAMES);
        buffer_head_init(&(cvchannel[i])->envAmp.head, &(prgmBuf[i]));
    }
    
    for(i=0; i<module_get_num_params(); i++) { param_setup(i, 0); }
}


void module_deinit(void) {}


extern u32 module_get_num_params(void) {
  return eParamNumParams;
}


fract32 (*get_processptr(u8 n))() {
    static fract32 (*process[])() =
    {
        process_cv,
        process_audio,
    };
    
    return (n < 1 || n > 1) ? *process[0] : *process[1];
}


void set_process(prgmCvChannel *cvchannel, u8 n) {
    cvchannel->process = get_processptr(n);
}


fract32 frame_next(prgmCvChannel *cvchannel) {
    return cvchannel->process(cvchannel);
}


void module_process_frame(void) {
    for(cvChn=0;cvChn<4;cvChn++)
    {
        if(!cvchannel[cvChn]->flag)
        {
            cvVal[cvChn] = frame_next(cvchannel[cvChn]);
            cv_update(cvChn, cvVal[cvChn]);
        }
        else if(cvchannel[cvChn]->flag)
        {
            out[cvChn] = frame_next(cvchannel[cvChn]);
        }
    }
}

/*
 void module_process_frame(void) {
 for(cvChn=0;cvChn<4;cvChn++)
 {
 //  env->source = env->f * env->t
 filter_1p_lo_in(&(cvchannel[cvChn]->fSlew), fix16_mul(cvchannel[cvChn]->f, cvchannel[cvChn]->t));
 cvchannel[cvChn]->envAmp.source = fix16_mul(cvchannel[cvChn]->f, cvchannel[cvChn]->t);
 
 if(filter_1p_sync(&(cvchannel[cvChn]->fSlew)))
 {
 if(cvchannel[cvChn]->envAmp.state == OFF)
 {
 ;;
 }
 else
 {
 cvVal[cvChn] = env_tcd_next(&(cvchannel[cvChn]->envAmp));
 cv_update(cvChn, cvVal[cvChn]);
 }
 }
 
 else
 {
 //  output encoder and update dac
 cvVal[cvChn] = filter_1p_lo_next(&(cvchannel[cvChn]->fSlew));
 cv_update(cvChn, cvVal[cvChn]);
 }
 }
 }
 */


fract32 process_cv(prgmCvChannel *cvchannel) {
    filter_1p_lo_in(&(cvchannel->fSlew), fix16_mul(cvchannel->f, cvchannel->t));
    cvchannel->envAmp.source = fix16_mul(cvchannel->f, cvchannel->t);
    
    if(filter_1p_sync(&(cvchannel->fSlew)))
    {
        if(cvchannel->envAmp.state == OFF)
        {
            ;;
        }
        else
        {
            //  output curve
            return env_tcd_next(&(cvchannel->envAmp));
        }
    }
    
    else
    {
        //  output encoder
        return filter_1p_lo_next(&(cvchannel->fSlew));
    }
    return 0;
}


fract32 process_audio(prgmCvChannel *cvchannel) {
    if(cvchannel->envAmp.state == OFF)
    {
        return 0;
    }
    else
    {
        return env_tcd_next(&(cvchannel->envAmp));
    }
}


void module_set_param(u32 idx, ParamValue v) {
    switch(idx) {
        case eParamFree0:
            cv_set_f(cvchannel[0], v);
            break;
        case eParamFree1:
            cv_set_f(cvchannel[1], v);
            break;
        case eParamFree2:
            cv_set_f(cvchannel[2], v);
            break;
        case eParamFree3:
            cv_set_f(cvchannel[3], v);
            break;
            
        case eParamTransposed0:
            cv_set_t(cvchannel[0], v);
            break;
        case eParamTransposed1:
            cv_set_t(cvchannel[1], v);
            break;
        case eParamTransposed2:
            cv_set_t(cvchannel[2], v);
            break;
        case eParamTransposed3:
            cv_set_t(cvchannel[3], v);
            break;
            
        case eParamCurveTime0:
            env_tcd_set_time(&(cvchannel[0]->envAmp), v);
            break;
        case eParamCurveTime1:
            env_tcd_set_time(&(cvchannel[1]->envAmp), v);
            break;
        case eParamCurveTime2:
            env_tcd_set_time(&(cvchannel[2]->envAmp), v);
            break;
        case eParamCurveTime3:
            env_tcd_set_time(&(cvchannel[3]->envAmp), v);
            break;
            
        case eParamCurve0:
            env_tcd_set_curve(&(cvchannel[0]->envAmp), v);
            break;
        case eParamCurve1:
            env_tcd_set_curve(&(cvchannel[1]->envAmp), v);
            break;
        case eParamCurve2:
            env_tcd_set_curve(&(cvchannel[2]->envAmp), v);
            break;
        case eParamCurve3:
            env_tcd_set_curve(&(cvchannel[3]->envAmp), v);
            break;

        case eParamCurveDest0:
            env_tcd_set_dest(&(cvchannel[0]->envAmp), v);
            break;
        case eParamCurveDest1:
            env_tcd_set_dest(&(cvchannel[1]->envAmp), v);
            break;
        case eParamCurveDest2:
            env_tcd_set_dest(&(cvchannel[2]->envAmp), v);
            break;
        case eParamCurveDest3:
            env_tcd_set_dest(&(cvchannel[3]->envAmp), v);
            break;

        case eParamCurveTrig0:
            env_tcd_set_trig(&(cvchannel[0]->envAmp), v);
            break;
        case eParamCurveTrig1:
            env_tcd_set_trig(&(cvchannel[1]->envAmp), v);
            break;
        case eParamCurveTrig2:
            env_tcd_set_trig(&(cvchannel[2]->envAmp), v);
            break;
        case eParamCurveTrig3:
            env_tcd_set_trig(&(cvchannel[3]->envAmp), v);
            break;
            
        case eParamFlag0:
            set_flag(cvchannel[0], v);
            set_process(cvchannel[0], v);
            break;
        case eParamFlag1:
            set_flag(cvchannel[1], v);
            set_process(cvchannel[1], v);
            break;
        case eParamFlag2:
            set_flag(cvchannel[2], v);
            set_process(cvchannel[2], v);
            break;
        case eParamFlag3:
            set_flag(cvchannel[3], v);
            set_process(cvchannel[3], v);
            break;
            
        default:
            break;
    }
}
