/* 
   sample.c
   aleph-bfin
*/

//-- aleph/common headers
#include "types.h"
#include <string.h>

//-- aleph/bfin-lib headers
// global variables
#include "bfin_core.h"
// cv output driver
#include "cv.h"
// gpio pin numbers
#include "gpio.h"
#include "fract_math.h"

//-- dsp class headers
// simple 1-pole integrator
#include "filter_1p.h"

// global declarations for module data
#include "module.h"
#include "params.h"

#define BUFFERSIZE 0x1000000
#define BUFFERSIZE_1 (0x1000000 - 1)
#define FR32_MAX1_2 0x3fffffff


typedef struct _sampleBuffer {
    volatile fract32 *data;             //pointer to data
    u32 samples;                        //count of samples
} sampleBuffer;

typedef struct _bufferHead {
    sampleBuffer *buf;                  //pointer to buffer
    s32 idx;                            //current idx
} bufferHead;

static void buffer_init(sampleBuffer *buf, volatile fract32 *data, u32 samples);
static void buffer_head_init(bufferHead *head, sampleBuffer *buf);
static s32 buffer_head_play(bufferHead *head);

typedef struct _mixData {
    ModuleData super;
    ParamData mParamData[eParamNumParams];
    
    volatile fract32 sampleBuffer[BUFFERSIZE];
} mixData;

ModuleData *gModuleData;
mixData *data;
sampleBuffer onchipbuffer[4];
u32 offset = 0;

bufferHead heads[4];
fract32 level[4] = { PARAM_AMP_MAX >> 2, PARAM_AMP_MAX >> 2, PARAM_AMP_MAX >> 2, PARAM_AMP_MAX >> 2 };
fract32 start[4] = { 0xff, 0xff, 0xff, 0xff };
fract32 loop[4] = { 48000, 48000, 48000, 48000 };
fract32 direction[4] = { 1, 1, 1, 1 };
fract32 trig[4] = { 0, 0, 0, 0 };

static inline void param_setup(u32 id, ParamValue v) {
    gModuleData->paramData[id].value = v;
    module_set_param(id, v);
}

void module_init(void) {
    data = (mixData*)SDRAM_ADDRESS;
    gModuleData = &(data->super);
    strcpy(gModuleData->name, "mix");
    gModuleData->paramData = data->mParamData;
    gModuleData->numParams = eParamNumParams;

    u32 n, i;

    for(n=0; n<4; n++)
    {
        buffer_init(&(onchipbuffer[n]), data->sampleBuffer, BUFFERSIZE);
    }
    
    for(n=0; n<4; n++)
    {
        buffer_head_init(&(heads[n]), &(onchipbuffer[n]));
    }

    for(i=0; i<BUFFERSIZE; i++)
    {
        data->sampleBuffer[i] = 0x00000000;
    }

    offset = 0;
    
    //  init parameters
    param_setup(eParam0, PARAM_AMP_MAX >> 2);
    param_setup(eParam1, 0);
    param_setup(eParam2, 48000);
    param_setup(eParam3, 1);
    param_setup(eParam4, 0);

    param_setup(eParam5, PARAM_AMP_MAX >> 2);
    param_setup(eParam6, 0);
    param_setup(eParam7, 48000);
    param_setup(eParam8, 1);
    param_setup(eParam9, 0);

    param_setup(eParam10, PARAM_AMP_MAX >> 2);
    param_setup(eParam11, 0);
    param_setup(eParam12, 48000);
    param_setup(eParam13, 1);
    param_setup(eParam14, 0);

    param_setup(eParam15, PARAM_AMP_MAX >> 2);
    param_setup(eParam16, 0);
    param_setup(eParam17, 48000);
    param_setup(eParam18, 1);
    param_setup(eParam19, 0);
    
//    for(n=0; n<module_get_num_params(); n++) param_setup(n, 0);}
}

// de-init (never actually used on blackfin, but maybe by emulator)
void module_deinit(void) {
}

// get number of parameters
u32 module_get_num_params(void) {
    return eParamNumParams;
}

void module_process_frame(void) {
    u8 c;
    static s64 x[4] = { FR32_MAX1_2, FR32_MAX1_2, FR32_MAX1_2, FR32_MAX1_2 };

    //  linear idx
    for (c=0; c<4; c++)
    {
        fract32 y0 = heads[c].idx;
        fract32 y1 = heads[c].idx + 2;
        
        if (direction[c] > 0)
        {
            x[c] += direction[c];
            
            if (x[c] > FR32_MAX)
            {
                x[c] = FR32_MAX1_2;
            }
            
            heads[c].idx = add_fr1x32(y0, mult_fr1x32x32(x[c], sub_fr1x32(y1, y0)));
            if (heads[c].idx > loop[c])
            {
                heads[c].idx = start[c];
            }
            if (trig[c])
            {
                heads[c].idx = start[c];
                trig[c] = 0;
            }
        }
        else
        {
            x[c] += -direction[c];
            
            if (x[c] > FR32_MAX)
            {
                x[c] = FR32_MAX1_2;
            }
            
            heads[c].idx = sub_fr1x32(y0, mult_fr1x32x32(x[c], sub_fr1x32(y1, y0)));
            if (heads[c].idx < start[c])
            {
                heads[c].idx = loop[c];
            }
            if (trig[c])
            {
                heads[c].idx = loop[c];
                trig[c] = 0;
            }
        }
        
        out[c] = mult_fr1x32x32(level[c], buffer_head_play(&(heads[c])));
    }
}

// sample offset
void module_set_offset(ParamValue v) {
    offset = v;
}

// sample
void module_set_sample(ParamValue v) {
    data->sampleBuffer[offset] = v;
    offset++;
}

// parameter set function
void module_set_param(u32 idx, ParamValue v) {
    fract32 tmp;
    
    switch(idx) {
            //  level 0
        case eParam0:
            level[0] = v;
            break;
            //  start 0
        case eParam1:
            tmp = v;
            if (tmp < 0xff) tmp = 0xff;
            if (tmp > BUFFERSIZE_1) tmp = BUFFERSIZE_1;
            start[0] = tmp;
            break;
            //  loop 0
        case eParam2:
            tmp = v;
            if (tmp < 0xff + 2) tmp = 0xff + 2;
            if (tmp > BUFFERSIZE_1) tmp = BUFFERSIZE_1;
            loop[0] = tmp;
            break;
            //  direction 0
        case eParam3:
            direction[0] = v;
            break;
            //  trig 0
        case eParam4:
            trig[0] = v;
            break;

            //  level 1
        case eParam5:
            level[1] = v;
            break;
            //  start 1
        case eParam6:
            tmp = v;
            if (tmp < 0xff) tmp = 0xff;
            if (tmp > BUFFERSIZE_1) tmp = BUFFERSIZE_1;
            start[1] = tmp;
            break;
            //  loop 1
        case eParam7:
            tmp = v;
            if (tmp < 0xff + 2) tmp = 0xff + 2;
            if (tmp > BUFFERSIZE_1) tmp = BUFFERSIZE_1;
            loop[1] = tmp;
            break;
            //  direction 1
        case eParam8:
            direction[1] = v;
            break;
            //  trig 1
        case eParam9:
            trig[1] = v;
            break;

            //  level 2
        case eParam10:
            level[2] = v;
            break;
            //  start 2
        case eParam11:
            tmp = v;
            if (tmp < 0xff) tmp = 0xff;
            if (tmp > BUFFERSIZE_1) tmp = BUFFERSIZE_1;
            start[2] = tmp;
            break;
            //  loop 2
        case eParam12:
            tmp = v;
            if (tmp < 0xff + 2) tmp = 0xff + 2;
            if (tmp > BUFFERSIZE_1) tmp = BUFFERSIZE_1;
            loop[2] = tmp;
            break;
            //  direction 2
        case eParam13:
            direction[2] = v;
            break;
            //  trig 2
        case eParam14:
            trig[2] = v;
            break;

            //  level 3
        case eParam15:
            level[3] = v;
            break;
            //  start 3
        case eParam16:
            tmp = v;
            if (tmp < 0xff) tmp = 0xff;
            if (tmp > BUFFERSIZE_1) tmp = BUFFERSIZE_1;
            start[3] = tmp;
            break;
            //  loop 3
        case eParam17:
            tmp = v;
            if (tmp < 0xff + 2) tmp = 0xff + 2;
            if (tmp > BUFFERSIZE_1) tmp = BUFFERSIZE_1;
            loop[3] = tmp;
            break;
            //  direction 3
        case eParam18:
            direction[3] = v;
            break;
            //  trig 3
        case eParam19:
            trig[3] = v;
            break;
 
        default:
            break;
    }
}

void buffer_init(sampleBuffer *buf, volatile fract32 *data, u32 samples) {
    buf->data = data;
    buf->samples = samples;
}

void buffer_head_init(bufferHead *head, sampleBuffer *buf) {
    head->buf = buf;
    head->idx = 0;
}

s32 buffer_head_play(bufferHead *head) {
    return head->buf->data[head->idx];
}
