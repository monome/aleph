//render.c
//aleph-prgm-avr32

//--- std headers
#include <string.h>

//debug
#include "print_funcs.h"

//avr32-lib
#include "fix.h"
#include "app.h"
#include "font.h"
#include "memory.h"
#include "region.h"
#include "screen.h"
#include "control.h"

//prgm
#include "tracker.h"
#include "pages.h"
#include "page_param.h"
#include "page_env.h"

#include "render.h" //includes region.h
#include "util.h"
#include "scale.h"

static char renderDummy[16];

//regions
static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};

static scroll bootScroll;

static const u8 numRegions_env = 6;
static const u8 numRegions_param = 6;

static region env[6] = {
    {.w=64, .h=24, .x = 0, .y = 0},     //track 1
    {.w=64, .h=24, .x = 64, .y = 0},    //track 2
    {.w=64, .h=24, .x = 0, .y = 24},    //track 3
    {.w=64, .h=24, .x = 64, .y = 24},   //track 4
    
    {.w=64, .h=8, .x = 0, .y = 48},     //step length, measure, tempo, buffer position
    {.w=64, .h=8, .x = 64, .y = 48},    //edit position
};

static region *envRegions[] = {
    &(env[0]),
    &(env[1]),
    &(env[2]),
    &(env[3]),
    &(env[4]),
    &(env[5]),
};

static region param[6] = {
    {.w=64, .h=24, .x = 0, .y = 0},     //track 1
    {.w=64, .h=24, .x = 64, .y = 0},    //track 2
    {.w=64, .h=24, .x = 0, .y = 24},    //track 3
    {.w=64, .h=24, .x = 64, .y = 24},   //track 4
    
    {.w=64, .h=8, .x = 0, .y = 48},     //step length, measure, tempo, buffer position
    {.w=64, .h=8, .x = 64, .y = 48},    //edit position
};

static region *paramRegions[] = {
    &(param[0]),
    &(param[1]),
    &(param[2]),
    &(param[3]),
    &(param[4]),
    &(param[5]),
};

//initialization, called by app_init()
void render_init(void) {
    region_alloc((region*)(&bootScrollRegion)); //BOOT: declared in region.h, calls mem_alloc() declared in memory.h
    scroll_init(&bootScroll, &bootScrollRegion);

    u32 i;
    for(i = 0; i<numRegions_param; i++) {
        region_alloc((region*)(paramRegions[i]));
    }
    for(i = 0; i<numRegions_env; i++) {
        region_alloc((region*)(envRegions[i]));
    }
}

void render_startup (void) {
    screen_clear();
    set_page(ePageEnv);
    
    //init with page env, fill parameters with startup values
    print_fix16(renderDummy, 0);
    
    print_fix16(renderTrig0, 0);
    print_fix16(renderTrig1, 0);
    print_fix16(renderTrig2, 0);
    print_fix16(renderTrig3, 0);
    
    print_fix16(renderTime0, 0);
    print_fix16(renderTime1, 0);
    print_fix16(renderTime2, 0);
    print_fix16(renderTime3, 0);

    print_fix16(renderEditPosition, 1 * 0x00010000);
    render_env();
}

void render_boot(const char* str) {
    scroll_string_front(&bootScroll, (char*)str);
}

void render_update(void) {
    region *r;
    u8 i;
    
    app_pause();
    
    if(bootScroll.reg->dirty) {
        scroll_draw(&bootScroll);
    }
    
    for(i = 0; i<numRegions_param; i++) {
        r = paramRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }
    
    for(i = 0; i<numRegions_env; i++) {
        r = envRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }

    app_resume();
}


//draw env page
void render_env(void) {
    u8 i;
    for(i = 0; i<numRegions_env; i++) {
        region_fill(&env[i], 0x0);
    }
    render_modes(editpos);
    
    /* region, string, offset x, offset y, color text, color background, size */
    region_string(&env[0], renderTrig0, 0, 0, 0xf, 0, 0);
    region_string(&env[1], renderTrig1, 0, 0, 0xf, 0, 0);
    region_string(&env[2], renderTrig2, 0, 0, 0xf, 0, 0);
    region_string(&env[3], renderTrig3, 0, 0, 0xf, 0, 0);
    
    region_string(&env[0], renderTime0, 0, 16, 0xf, 0, 0);
    region_string(&env[1], renderTime1, 0, 16, 0xf, 0, 0);
    region_string(&env[2], renderTime2, 0, 16, 0xf, 0, 0);
    region_string(&env[3], renderTime3, 0, 16, 0xf, 0, 0);

    render_steplength_env(editpos);
    region_string(&env[5], renderEditPosition, 0, 0, 0xf, 0, 0);
}

void render_trig(void) {
    region_string(&env[0], renderTrig0, 0, 0, 0xf, 0, 0);
    region_string(&env[1], renderTrig1, 0, 0, 0xf, 0, 0);
    region_string(&env[2], renderTrig2, 0, 0, 0xf, 0, 0);
    region_string(&env[3], renderTrig3, 0, 0, 0xf, 0, 0);
}

void render_modes(u8 pos) {
    render_mode(0, track[0]->m[pos]);
    render_mode(1, track[1]->m[pos]);
    render_mode(2, track[2]->m[pos]);
    render_mode(3, track[3]->m[pos]);
}

//  MAX 12 char in a name for now..
void render_mode(u8 track, s32 name) {
    static char *nameptr[] = {
        "OFF",              //0
        "HOLD",             //1
        "TRIG",             //2
        "GATE",             //3
        "NOISE",            //4
        "one",              //5
        "loop",             //6
        "wav",              //7
        "noise",            //8
        "record",           //9
        "TRIGrecord",       //10
        "[aux master]"      //11
    };
    
    region_string(&env[track], "████████████", 0, 8, 0, 0, 0);
    region_string(&env[track], nameptr[name], 0, 8, 0xf, 0, 0);
}

void render_time(void) {
    region_string(&env[0], renderTime0, 0, 16, 0xf, 0, 0);
    region_string(&env[1], renderTime1, 0, 16, 0xf, 0, 0);
    region_string(&env[2], renderTime2, 0, 16, 0xf, 0, 0);
    region_string(&env[3], renderTime3, 0, 16, 0xf, 0, 0);
}

void render_aux_env(u8 chn) {
    static char *auxptr[] = {
        "AUX 1 :",
        "AUX 2 :",
        "AUX 3 :",
        "AUX 4 :"
    };
    
    region_string(&env[4], "████████████", 0, 0, 0, 0, 0);
    region_string(&env[4], auxptr[chn], 0, 0, 0xf, 0, 0);
    
    if(chn == 0) region_string(&env[5], renderLevel0, 0, 0, 0xf, 0, 0);
    else if(chn == 1) region_string(&env[5], renderLevel1, 0, 0, 0xf, 0, 0);
    else if(chn == 2) region_string(&env[5], renderLevel2, 0, 0, 0xf, 0, 0);
    else if(chn == 3) region_string(&env[5], renderLevel3, 0, 0, 0xf, 0, 0);
    else ;
}

void render_steplength_env(u8 pos) {
    static char *lengthptr[] = {
        "SKIP",
        "1/32",
        "1/16",
        "1/8",
        "1/4",
        "1/2",
        "ALIGN"
    };
    
    region_string(&env[4], "████████████", 0, 0, 0, 0, 0);
    region_string(&env[4], lengthptr[n_scale_lookup[pos]], 0, 0, 0xf, 0, 0);
}

void render_measure_env(u8 pos) {
    static char *measureptr[] = {
        "1 : 1",
        "2 : 2",
        "4 : 4",
        "8 : 8",
        "16 : 16",
        "32 : 32"
    };
    
    region_string(&env[4], "████████████", 0, 0, 0, 0, 0);
    region_string(&env[4], measureptr[pos], 0, 0, 0xf, 0, 0);
}

void render_bufferposition_env(u8 pos) {
    static char *bufferptr[] = {
        "track 1",
        "track 2",
        "track 3",
        "track 4"
    };
    
    region_string(&env[5], "████████████", 0, 0, 0, 0, 0);
    region_string(&env[5], bufferptr[pos], 0, 0, 0xf, 0, 0);
}

void render_tempo_env(void) {
    region_string(&env[4], renderTempo, 0, 0, 0xf, 0, 0);
}

void render_counters_env(void) {
    render_steplength_env(editpos);
    region_string(&env[5], renderEditPosition, 0, 0, 0xf, 0, 0);
}

//draw param page
void render_param(void) {
    u8 i;
    for(i = 0; i<numRegions_param; i++) {
        region_fill(&param[i], 0x0);
    }
    
    render_track0(track[0]->m[editpos]);
    render_track1(track[1]->m[editpos]);
    render_track2(track[2]->m[editpos]);
    render_track3(track[3]->m[editpos]);
    
    render_steplength_env(editpos);
    region_string(&param[5], renderEditPosition, 0, 0, 0xf, 0, 0);
}

//render tracks based on mode
void render_track0(u8 mode) {
    switch (mode) {
        //OFF
        case 0:
            region_string(&param[0], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        //HOLD
        case 1:
            print_fix16(renderFrequency0, track[0]->pF[editpos]);
            region_string(&param[0], renderFrequency0, 0, 0, 0xf, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        //TRIG
        case 2:
            region_string(&param[0], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        //GATE
        case 3:
            region_string(&param[0], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;
        
        //NOISE
        case 4:
            region_string(&param[0], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        //one
        case 5:
            print_fix16(renderPosition0, track[0]->pP[editpos]);
            region_string(&param[0], renderPosition0, 0, 0, 0xf, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;

        //loop
        case 6:
            print_fix16(renderPosition0, track[0]->pP[editpos]);
            region_string(&param[0], renderPosition0, 0, 0, 0xf, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        //wav
        case 7:
            render_sample(0, track[0]->pS[editpos]);
            print_fix16(renderPosition0, track[0]->pP[editpos]);
            region_string(&param[0], renderPosition0, 0, 8, 0xf, 0, 0);
            break;

        //noise
        case 8:
            region_string(&param[0], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;

        //rec
        case 9:
            render_input(0, track[0]->pI[editpos]);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        //TRIGrec
        case 10:
            render_input(0, track[0]->pI[editpos]);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        //aux
        case 11:
            region_string(&param[0], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[0], renderDummy, 0, 8, 0x1, 0, 0);
            break;

        default:
            break;
    }
}

void render_track1(u8 mode) {
    switch (mode) {
            //OFF
        case 0:
            region_string(&param[1], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;

            //HOLD
        case 1:
            print_fix16(renderFrequency1, track[1]->pF[editpos]);
            region_string(&param[1], renderFrequency1, 0, 0, 0xf, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //TRIG
        case 2:
            region_string(&param[1], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //GATE
        case 3:
            region_string(&param[1], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //NOISE
        case 4:
            region_string(&param[1], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //one
        case 5:
            print_fix16(renderPosition1, track[1]->pP[editpos]);
            region_string(&param[1], renderPosition1, 0, 0, 0xf, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //loop
        case 6:
            print_fix16(renderPosition1, track[1]->pP[editpos]);
            region_string(&param[1], renderPosition1, 0, 0, 0xf, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //wav
        case 7:
            render_sample(1, track[1]->pS[editpos]);
            print_fix16(renderPosition1, track[1]->pP[editpos]);
            region_string(&param[1], renderPosition1, 0, 8, 0xf, 0, 0);
            break;
            
            //noise
        case 8:
            region_string(&param[1], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //rec
        case 9:
            render_input(1, track[1]->pI[editpos]);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //TRIGrec
        case 10:
            render_input(1, track[1]->pI[editpos]);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //aux
        case 11:
            region_string(&param[1], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[1], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        default:
            break;
    }
}

void render_track2(u8 mode) {
    switch (mode) {
            //OFF
        case 0:
            region_string(&param[2], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //HOLD
        case 1:
            print_fix16(renderFrequency2, track[2]->pF[editpos]);
            region_string(&param[2], renderFrequency2, 0, 0, 0xf, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //TRIG
        case 2:
            region_string(&param[2], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //GATE
        case 3:
            region_string(&param[2], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //NOISE
        case 4:
            region_string(&param[2], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //one
        case 5:
            print_fix16(renderPosition2, track[2]->pP[editpos]);
            region_string(&param[2], renderPosition2, 0, 0, 0xf, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //loop
        case 6:
            print_fix16(renderPosition2, track[2]->pP[editpos]);
            region_string(&param[2], renderPosition2, 0, 0, 0xf, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //wav
        case 7:
            render_sample(2, track[2]->pS[editpos]);
            print_fix16(renderPosition2, track[2]->pP[editpos]);
            region_string(&param[2], renderPosition2, 0, 8, 0xf, 0, 0);
            break;
            
            //noise
        case 8:
            region_string(&param[2], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //rec
        case 9:
            render_input(2, track[2]->pI[editpos]);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //TRIGrec
        case 10:
            render_input(2, track[2]->pI[editpos]);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //aux
        case 11:
            region_string(&param[2], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[2], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        default:
            break;
    }
}

void render_track3(u8 mode) {
    switch (mode) {
            //OFF
        case 0:
            region_string(&param[3], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //HOLD
        case 1:
            print_fix16(renderFrequency3, track[3]->pF[editpos]);
            region_string(&param[3], renderFrequency3, 0, 0, 0xf, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //TRIG
        case 2:
            region_string(&param[3], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //GATE
        case 3:
            region_string(&param[3], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //NOISE
        case 4:
            region_string(&param[3], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //one
        case 5:
            print_fix16(renderPosition3, track[3]->pP[editpos]);
            region_string(&param[3], renderPosition3, 0, 0, 0xf, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //loop
        case 6:
            print_fix16(renderPosition3, track[3]->pP[editpos]);
            region_string(&param[3], renderPosition3, 0, 0, 0xf, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //wav
        case 7:
            render_sample(3, track[3]->pS[editpos]);
            print_fix16(renderPosition3, track[3]->pP[editpos]);
            region_string(&param[3], renderPosition3, 0, 8, 0xf, 0, 0);
            break;
            
            //noise
        case 8:
            region_string(&param[3], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //rec
        case 9:
            render_input(3, track[3]->pI[editpos]);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //TRIGrec
        case 10:
            render_input(3, track[3]->pI[editpos]);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
            //aux
        case 11:
            region_string(&param[3], renderDummy, 0, 0, 0x1, 0, 0);
            region_string(&param[3], renderDummy, 0, 8, 0x1, 0, 0);
            break;
            
        default:
            break;
    }
}

void render_sample(u8 track, s32 sample) {
    region_string(&param[track], "████████████", 0, 0, 0, 0, 0);
    region_string(&param[track], sample_name[sample-4], 0, 0, 0xf, 0, 0);
}

void render_input(u8 track, s32 input) {
    static char *inputptr[] = {
        "In 1",
        "In 2",
        "In 1+2",
        "Aux",
        "Out 1",
        "Out 2",
        "Out 3",
        "Out 4"
    };
    
    region_string(&param[track], "████████████", 0, 0, 0, 0, 0);
    region_string(&param[track], inputptr[input], 0, 0, 0xf, 0, 0);
}

void render_aux_param(u8 chn) {
    static char *auxptr[] = {
        "AUX TRK 1 :",
        "AUX TRK 2 :",
        "AUX TRK 3 :",
        "AUX TRK 4 :"
    };
    
    region_string(&param[4], "████████████", 0, 0, 0, 0, 0);
    region_string(&param[4], auxptr[chn], 0, 0, 0xf, 0, 0);
    
    if(chn == 0) region_string(&param[5], renderLevel0, 0, 0, 0xf, 0, 0);
    else if(chn == 1) region_string(&param[5], renderLevel1, 0, 0, 0xf, 0, 0);
    else if(chn == 2) region_string(&param[5], renderLevel2, 0, 0, 0xf, 0, 0);
    else if(chn == 3) region_string(&param[5], renderLevel3, 0, 0, 0xf, 0, 0);
    else ;
}

void render_steplength_param(u8 pos) {
    static char *lengthptr[] = {
        "SKIP",
        "1/32",
        "1/16",
        "1/8",
        "1/4",
        "1/2",
        "1/32",
        "ALIGN"
    };
    
    region_string(&param[4], "████████████", 0, 0, 0, 0, 0);
    region_string(&param[4], lengthptr[n_scale_lookup[pos]], 0, 0, 0xf, 0, 0);
}

void render_measure_param(u8 pos) {
    static char *measureptr[] = {
        "1 : 1",
        "2 : 2",
        "4 : 4",
        "8 : 8",
        "16 : 16",
        "32 : 32"
    };
    
    region_string(&param[4], "████████████", 0, 0, 0, 0, 0);
    region_string(&param[4], measureptr[pos], 0, 0, 0xf, 0, 0);
}

void render_bufferposition_param(u8 pos) {
    static char *bufferptr[] = {
        "track 1",
        "track 2",
        "track 3",
        "track 4"
    };
    
    region_string(&param[5], "████████████", 0, 0, 0, 0, 0);
    region_string(&param[5], bufferptr[pos], 0, 0, 0xf, 0, 0);
}

void render_tempo_param(void) {
    region_string(&param[4], renderTempo, 0, 0, 0xf, 0, 0);
}

void render_counters_param(void) {
    render_steplength_param(editpos);
    region_string(&param[5], renderEditPosition, 0, 0, 0xf, 0, 0);
}

