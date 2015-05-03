//render.c
//aleph-prgm-avr32
/* region, string, offset x, offset y, color text, color background, size */

#include "render.h"

//regions
static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};
static scroll bootScroll;

static region lineRegion = {.w=128, .h=8, .x = 0, .y = 0};

static region sequencerRegion = {.w = 128, .h = 56, .x = 0, .y = 0};
static scroll seqScroll;

static const u8 numRegions_trk = 12;
static const u8 numRegions_mix = 6;
static const u8 numRegions_seq = 4;
static const u8 numRegions_gen = 12;

static region trk[12] = {
    {.w=32, .h=8, .x = 64, .y = 56},    //0 track
    {.w=32, .h=8, .x = 96, .y = 56},    //1 mode | mute
    
    {.w=64, .h=24, .x = 0, .y = 0},     //2 input
    {.w=64, .h=8, .x = 64, .y = 0},     //3 input level
    {.w=64, .h=8, .x = 64, .y = 8},     //4 aux 1 level
    {.w=64, .h=8, .x = 64, .y = 16},    //5 aux 2 level
    {.w=64, .h=40, .x = 0, .y = 24},    //6 mix pan
    {.w=64, .h=32, .x = 64, .y = 24},   //7 mix level

    {.w=64, .h=8, .x = 0, .y = 32},     //8 param 1 name
    {.w=64, .h=8, .x = 64, .y = 32},    //9 param 1 value
    {.w=64, .h=8, .x = 0, .y = 40},     //10 param 2 name
    {.w=64, .h=8, .x = 64, .y = 40},    //11 param 2 value
};

static region *trkRegions[] = {
    &(trk[0]),
    &(trk[1]),
    &(trk[2]),
    &(trk[3]),
    &(trk[4]),
    &(trk[5]),
    &(trk[6]),
    &(trk[7]),
    &(trk[8]),
    &(trk[9]),
    &(trk[10]),
    &(trk[11]),
};

static region mix[6] = {
    {.w=64, .h=8, .x = 0, .y = 0},      //0 aux 1 pan
    {.w=64, .h=56, .x = 0, .y = 8},     //1 aux 2 pan
    
    {.w=64, .h=8, .x = 64, .y = 0},     //2 direct output 1
    {.w=64, .h=8, .x = 64, .y = 8},     //3 direct output 2
    {.w=64, .h=40, .x = 64, .y = 16},   //4 mix level
    
    {.w=64, .h=8, .x = 64, .y = 56},    //5 app version
};

static region *mixRegions[] = {
    &(mix[0]),
    &(mix[1]),
    &(mix[2]),
    &(mix[3]),
    &(mix[4]),    
    &(mix[5]),    
};

static region seq[4] = {
    {.w=32, .h=8, .x = 64, .y = 56},    //0 track
    {.w=16, .h=8, .x = 96, .y = 56},    //1 measure
    {.w=16, .h=8, .x = 112, .y = 56},   //2 length
    {.w=32, .h=8, .x = 96, .y = 56},    //3 mute
};

static region *seqRegions[] = {
    &(seq[0]),
    &(seq[1]),
    &(seq[2]),
    &(seq[3]),
};

static region gn[12] = {
    {.w=64, .h=8, .x = 0, .y = 0},      //0 gen
    {.w=64, .h=8, .x = 64, .y = 0},     //1 gen type
    {.w=64, .h=8, .x = 0, .y = 8},      //2 lo
    {.w=64, .h=8, .x = 64, .y = 8},     //3 hi
    {.w=64, .h=8, .x = 0, .y = 16},     //4 rate
    {.w=64, .h=8, .x = 64, .y = 16},    //5 att
    {.w=64, .h=8, .x = 0, .y = 24},     //6 dens
    {.w=64, .h=8, .x = 64, .y = 24},    //7 dur
    {.w=64, .h=16, .x = 0, .y = 32},    //8 destination
    {.w=64, .h=16, .x = 64, .y = 32},   //9 value
    
    {.w=64, .h=8, .x = 0, .y = 48},     //10 track
    {.w=64, .h=8, .x = 64, .y = 48},    //11 step
};

static region *genRegions[12] = {
    &(gn[0]),
    &(gn[1]),
    &(gn[2]),
    &(gn[3]),
    &(gn[4]),
    &(gn[5]),
    &(gn[6]),
    &(gn[7]),
    &(gn[8]),
    &(gn[9]),
    &(gn[10]),
    &(gn[11]),
};

//initialization, called by app_init()
void render_init(void) {
    region_alloc((region*)(&bootScrollRegion)); //BOOT: declared in region.h, calls mem_alloc() declared in memory.h
    scroll_init(&bootScroll, &bootScrollRegion);
    
    region_alloc((region*)(&lineRegion));
    
    region_alloc((region*)(&sequencerRegion));
    scroll_init(&seqScroll, &sequencerRegion);

    u32 i;
    for(i = 0; i<numRegions_trk; i++) {
        region_alloc((region*)(trkRegions[i]));
    }
    for(i = 0; i<numRegions_mix; i++) {
        region_alloc((region*)(mixRegions[i]));
    }
    for(i = 0; i<numRegions_seq; i++) {
        region_alloc((region*)(seqRegions[i]));
    }
    for(i = 0; i<numRegions_gen; i++) {
        region_alloc((region*)(genRegions[i]));
    }
}

void render_startup (void) {
    trkpage = 0;
    screen_clear();
    set_page(ePageMix);
    render_mix();
}

void region_update(region *r) {
    if(r->dirty) {
        screen_draw_region(r->x, r->y, r->w, r->h, r->data);
        r->dirty = 0;
    }
}

extern void render_boot(const char *str) {
    int i;
    u8 *p = bootScroll.reg->data;
    //  first kind of dim the old stuff in the scroll region
    for(i=0; i<bootScroll.reg->len; i++)
    {
        if(*p > 0x4)
        {
            *p = 0x4;
        }
        p++;
    }
    scroll_string_front(&bootScroll, (char*)str);
    region_update(bootScroll.reg);
}
/*
void render_boot(const char* str) {
    scroll_string_front(&bootScroll, (char*)str);
}
*/
void render_update(void) {
    region *r;
    u8 i;
    
    app_pause();
    
    if(bootScroll.reg->dirty) {
        scroll_draw(&bootScroll);
    }
    
    if(seqScroll.reg->dirty) {
        scroll_draw(&seqScroll);
    }

    for(i = 0; i<numRegions_trk; i++) {
        r = trkRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }
    
    for(i = 0; i<numRegions_mix; i++) {
        r = mixRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }
    
    for(i = 0; i<numRegions_seq; i++) {
        r = seqRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }
    
    for(i = 0; i<numRegions_gen; i++) {
        r = genRegions[i];
        if(r->dirty) {
            screen_draw_region(r->x, r->y, r->w, r->h, r->data);
            r->dirty = 0;
        }
    }
    
    app_resume();
}


//draw trk page
static void render_trk_name(u8 region, s32 name);
static void render_trk_input(u8 region, s32 input);
static void render_trk_mode(u8 region, s32 mode);
static void render_trk_param(u8 track, u8 mode);

void render_trk(void) {
    u8 i;
    for(i = 0; i<numRegions_trk; i++) {
        region_fill(&trk[i], 0x0);
    }
    render_track(trkpage);
}

void render_track(u8 t) {
    render_trk_name(0, t);
    
    if (!track[t]->mutemix)
    {
        region_fill(&trk[1], 0x0);
        render_trk_mode(1, track[t]->m);
    }
    if (track[t]->mutemix)
    {
        region_fill(&trk[1], 0x0);
        region_string(&trk[1], "MUTE", 0, 0, 0xf, 0, 0);
    }

    render_trk_input(2, track[t]->input);
    print_fix16(renderInputLevel[t], track[t]->inL);
    region_string(&trk[3], renderInputLevel[t], 0, 0, 0xf, 0, 0);
    
    print_fix16(renderAux1Level[t], track[t]->aux1);
    region_string(&trk[4], renderAux1Level[t], 0, 0, 0xf, 0, 0);
    print_fix16(renderAux2Level[t], track[t]->aux2);
    region_string(&trk[5], renderAux2Level[t], 0, 0, 0xf, 0, 0);
    
    print_fix16(renderMixPan[t], track[t]->pan);
    region_string(&trk[6], renderMixPan[t], 0, 0, 0xf, 0, 0);
    
    print_fix16(renderMixLevel[t], track[t]->mix);
    region_string(&trk[7], renderMixLevel[t], 0, 0, 0xf, 0, 0);

    render_trk_param(t, track[t]->m);
    render_trk_param(t, track[t]->m);
}

void render_trk_name(u8 region, s32 name) {
    static char *nameptr[] = {
        "Trk 1",
        "Trk 2",
        "Trk 3",
        "Trk 4",
        "Trk 5",
        "Trk 6",
        "Trk 7",
        "Trk 8"
    };
    
    region_string(&trk[region], nameptr[name], 0, 0, 0xf, 0, 0);
}

void render_trk_mode(u8 region, s32 mode) {
    static char *nameptr[] = {
        "off",              //0
        "trig",             //1 trig
        "loop",             //2 loop
        "[amp]*",           //3 [amp]
        "[dly]",            //4 [dly]
        "[thru]"            //5 [bp]
    };
    
    region_fill(&trk[region], 0x0);
    region_string(&trk[region], nameptr[mode], 0, 0, 0xf, 0, 0);
}

void render_trk_input(u8 region, s32 input) {
    static char *inputptr[] = {
        "Off",
        "In 1",
        "In 2",
        "In 3",
        "In 4",
        "Trk 1",
        "Trk 1 REV",
        "Trk 2",
        "Trk 2 REV",
        "Trk 3",
        "Trk 3 REV",
        "Trk 4",
        "Trk 4 REV",
        "Trk 5",
        "Trk 5 REV",
        "Trk 6",
        "Trk 6 REV",
        "Trk 7",
        "Trk 7 REV",
        "Trk 8",
        "Trk 8 REV",
        "Aux 1",
        "Aux 1 REV",
        "Aux 2",
        "Aux 2 REV",
        "Aux mix"
    };
    
    region_fill(&trk[region], 0x0);
    region_string(&trk[region], inputptr[input], 0, 0, 0xf, 0, 0);
}

void render_trk_param(u8 t, u8 m) {
    region_fill(&trk[8], 0x0);
    region_fill(&trk[9], 0x0);
    region_fill(&trk[10], 0x0);
    region_fill(&trk[11], 0x0);

    switch (m) {
            //Off
        case 0:
            break;
            
            //trig
        case 1:
            break;
            
            //loop
        case 2:
            break;
            
            //amp
        case 3:
            break;
            
            //dly
        case 4:
            region_string(&trk[8], "delay", 0, 0, 0xf, 0, 0);
            print_fix16(renderParam1[t], track[t]->uP - sample[t]->offset);
            region_string(&trk[9], renderParam1[t], 0, 0, 0xf, 0, 0);
            region_string(&trk[10], "feedback", 0, 0, 0xf, 0, 0);
            print_fix16(renderParam2[t], track[t]->sP);
            region_string(&trk[11], renderParam2[t], 0, 0, 0xf, 0, 0);
            break;
            
            //bp
        case 5:
            break;
            
            //noise
        case 6:
            break;
                        
        default:
            break;
    }
}

//draw mix page
static void render_direct_output(u8 region, s32 output);

void render_mix(void) {
    u8 i;
    for(i = 0; i<numRegions_mix; i++) {
        region_fill(&mix[i], 0x0);
    }
        
    print_fix16(renderAux1Pan, master->pan1);
    region_string(&mix[0], renderAux1Pan, 0, 0, 0xf, 0, 0);

    print_fix16(renderAux2Pan, master->pan2);
    region_string(&mix[1], renderAux2Pan, 0, 0, 0xf, 0, 0);

    render_direct_output(2, master->out3);
    render_direct_output(3, master->out4);

    print_fix16(renderMasterLevel, master->output);
    region_string(&mix[4], renderMasterLevel, 0, 0, 0xf, 0, 0);
    
    region_string(&mix[5], "     [prgm 0.0.8]", 0, 0, 0x1, 0, 0);
}

void render_direct_output(u8 region, s32 output) {
    static char *outputptr[] = {
        "Off",
        "Trk 1",
        "Trk 2",
        "Trk 3",
        "Trk 4",
        "Trk 5",
        "Trk 6",
        "Trk 7",
        "Trk 8",
        "Aux 1",
        "Aux 2"
    };
    
    region_string(&mix[region], outputptr[output], 0, 0, 0xf, 0, 0);
}


//draw seq page
static void render_line(u8 track, s32 editpos);
static void redraw_rows(void);
static void render_to_scroll_line(u8 offset);

static void clearln( void);
static void appendln_idx_lj(u16 val);
static void appendln(const char *str);
extern void endln( void);

char lineBuf[LINEBUF_LEN];
char numBuf[NUMBUF_LEN];
static char *pline = lineBuf;
static char *const pLineEnd = lineBuf + LINEBUF_LEN - 1;

// byte offsets for text lines
static u32 scrollLines[7] = {
    0,
    1024,
    2048,
    3072,
    4096,
    5120,
    6144
};

void render_seq_name(u8 region, s32 name);

void render_seq(void) {
    u8 i;
    u8 t = trkpage;
    for(i = 0; i<numRegions_seq; i++) {
        region_fill(&seq[i], 0x0);
    }    
    scroll_sequence(t);
    render_seqtrack(t);
}

void scroll_sequence(u8 t) {
    region_fill(&sequencerRegion, 0x0);
    redraw_rows();
}

void render_line(u8 t, s32 i) {
    region_fill(&lineRegion, 0x0);
    
    if (i == editpos)
    {
        clearln();
        appendln_idx_lj((u8)i + 1);
        endln();
        font_string_region_clip(&lineRegion, lineBuf, 0, 0, 0xf, 0);
        
        clearln();
        appendln(sample_name[track[t]->s[i]-8]);
        endln();
        font_string_region_clip(&lineRegion, lineBuf, LINE_VAL_POS_PARAM1, 0, 0xf, 0);
        
        clearln();
        print_fix16(renderTrackLevel, track[t]->outL[i]);
        appendln(renderTrackLevel);
        endln();
        font_string_region_clip(&lineRegion, lineBuf, LINE_VAL_POS_PARAM2, 0, 0xf, 0);
    }
    else
    {
        clearln();
        appendln_idx_lj((u8)i + 1);
        endln();
        font_string_region_clip(&lineRegion, lineBuf, 0, 0, 0x7, 0);
        
        clearln();
        appendln(sample_name[track[t]->s[i]-8]);
        endln();
        font_string_region_clip(&lineRegion, lineBuf, LINE_VAL_POS_PARAM1, 0, 0x7, 0);
        
        clearln();
        print_fix16(renderTrackLevel, track[t]->outL[i]);
        appendln(renderTrackLevel);
        endln();
        font_string_region_clip(&lineRegion, lineBuf, LINE_VAL_POS_PARAM2, 0, 0x7, 0);
    }
}

void redraw_rows(void) {
    u8 i;
    s32 n;
    const s32 max = track[trkpage]->len - 1;
    
    for(i=0; i<7; i++)
    {
        n = editpos + (i - SCROLL_OFFSET);
        if (n >= 0 && n <= max)
        {
            render_line(trkpage, n);
            render_to_scroll_line(i);
        }
        else
        {
            region_fill(&lineRegion, 0x0);
            render_to_scroll_line(i);
        }
    }
}

void render_to_scroll_line(u8 offset) {
    u8 *src;
    u8 *dst;
    u8 *dstMax;
    
    // data offset in scroll
    s32 dstOff = seqScroll.byteOff + scrollLines[offset];
    while(dstOff >= sequencerRegion.len)
    {
        dstOff -= sequencerRegion.len;
    }
    dst = sequencerRegion.data + dstOff;

    // setup copy
    src = lineRegion.data;
    dstMax = dst + sequencerRegion.len;
    
    // copy
    while(dst < dstMax)
    {
        *dst = *src;
        ++src;
        ++dst;
    }
    sequencerRegion.dirty = 1;
}

// clear line buffer
inline void clearln(void) {
    pline = lineBuf;
    memset(pline, ' ', LINEBUF_LEN);
}

// append int to line buffer
inline void appendln_idx_lj(u16 val) {
#if 0
    pline = atoi_idx(pline, val);
    #else
    u16 dig = 0;
    u16 rem = 0;
    // 3 digits only
    if(val > 999) { rem = 999; } else { rem = val; }
    
    dig = val / 100U;
    rem -= dig * 100U;
    *pline = '0' + dig;
    ++pline;
    
    dig = rem / 10U;
    rem -= dig * 10U;
    *pline = '0' + dig;
    ++pline;
    
    *pline = '0' + rem;
    ++pline;
    #endif
}

// append to line buffer
inline void appendln(const char* str) {
    while((*str != 0) && (pline <= pLineEnd)) {
        *pline = *str;
        ++pline;
        ++str;
    }
}

inline void endln(void) {
    *(pline) = '\0';
}

void render_seq_name(u8 region, s32 name) {
    static char *nameptr[] = {
        "Trk 1",
        "Trk 2",
        "Trk 3",
        "Trk 4",
        "Trk 5",
        "Trk 6",
        "Trk 7",
        "Trk 8"
    };
    
    region_string(&seq[region], nameptr[name], 0, 0, 0xf, 0, 0);
}

void render_seqtrack(u8 t) {    
    render_seq_name(0, t);
    
    if (!track[t]->mutetrk)
    {
        clearln();
        appendln_idx_lj((u8)track[t]->msr);
        endln();
        font_string_region_clip(&seq[1], lineBuf, 0, 0, 0xf, 0);
        seq[1].dirty = 1;
        
        clearln();
        appendln_idx_lj((u8)track[t]->len);
        endln();
        font_string_region_clip(&seq[2], lineBuf, 0, 0, 0xf, 0);
        seq[2].dirty = 1;
    }
    
    if (track[t]->mutetrk)
    {
        region_fill(&seq[3], 0x0);
        region_string(&seq[3], "MUTE", 0, 0, 0xf, 0, 0);
    }
}


//draw gen page
static void render_gsdestname(u8 destination);
static void render_gsname(u8 genpos);
static void render_dstmode(s32 mode);

void render_gen(void) {
//    region_string(&gn[0], renderGenPos, 0, 0, 0xf, 0, 0);
//    render_gsname(gen->g[genpos]);
    
    region_string(&gn[2], renderEnvPhase, 0, 0, 0xf, 0, 0);
    region_string(&gn[3], renderEnvFreq, 0, 0, 0xf, 0, 0);

    region_string(&gn[4], renderEnvRate, 0, 0, 0xf, 0, 0);
    region_string(&gn[5], renderEnvAtten, 0, 0, 0xf, 0, 0);

    region_string(&gn[6], renderGenDens, 0, 0, 0xf, 0, 0);
    region_string(&gn[7], renderGenDur, 0, 0, 0xf, 0, 0);
    
    render_gsdest(gen->gD[genpos]);

    render_gstrackname(trackpos);    
    region_string(&gn[11], renderEditPos, 0, 0, 0xf, 0, 0);
}

void render_gen_param(u8 p) {
//    render_gsname(gen->g[p]);
    region_string(&gn[2], renderEnvPhase, 0, 0, 0xf, 0, 0);
    region_string(&gn[3], renderEnvFreq, 0, 0, 0xf, 0, 0);
    region_string(&gn[4], renderEnvRate, 0, 0, 0xf, 0, 0);
    region_string(&gn[5], renderEnvAtten, 0, 0, 0xf, 0, 0);
    region_string(&gn[6], renderGenDens, 0, 0, 0xf, 0, 0);
    region_string(&gn[7], renderGenDur, 0, 0, 0xf, 0, 0);
}

void render_gsname(u8 g) {
    static char *genptr[] = {
        "halfsine",
        "gaussian", //gaussian
        "qausi-gauss", //quasi-gauss
        "ramp", //trapezoid
        "tri", //triangular
        "sinc", //sinc
        "expodec", //expodec
        "Rexpodec", //Rexpodec
        "lindec",
        "Rlindec"
    };
    
    region_fill(&gn[1], 0x0);
    region_string(&gn[1], genptr[g], 0, 0, 0xf, 0, 0);
}

void render_gstrackname(u8 t) {
    static char *trackptr[] = {
        "TRACK 1",
        "TRACK 2",
        "TRACK 3",
        "TRACK 4",
        "TRACK 5",
        "TRACK 6",
        "TRACK 7",
        "TRACK 8"
    };
    
    region_fill(&gn[10], 0x0);
    region_string(&gn[10], trackptr[t], 0, 0, 0xf, 0, 0);
}

void render_gsdestname(u8 d) {
    static char *dstptr[] = {
        "TRIG",
        "TIME",
        "MODE",
        "SAMPLE",
        "OFFSET A",
        "OFFSET B",
        "LEVEL",
        "FREQUENCY"
    };
    
    region_fill(&gn[8], 0x0);
    region_string(&gn[8], dstptr[d], 0, 0, 0xf, 0, 0);
}

void render_dstmode(s32 mode) {
    static char *nameptr[] = {
        "OFF",              //0
        "TRIG",             //1
        "GATE",             //2
        "*",                //3 TCD
        "wav",              //4
        "noise",            //5
        "play",             //6
        "rec",              //7
        "TRIGrec",          //8
        "*",                //9 [bp]
        "[delay]",          //10
        "*",                //11 [bp->fb(loop)]
        "*",                //12 [fb(loop->bp)]
        "*",                //13 [fb(rvb)]
        "*",                //14 [rvb->bp]
        "*",                //15 [3bp]
        "[insert mix]",     //16
        "[aux master]",     //17
        "<LEN>",            //18
        "*"                 //19 <EXT>
    };
    
    region_string(&gn[9], nameptr[mode], 0, 0, 0xf, 0, 0);
}

void render_gsdest(u8 d) {
    u8 t = trackpos;
    render_gsdestname(d);
    
    //trig
    if (d == 0)
    {
        region_fill(&gn[9], 0x0);
//        print_fix16(renderTrig[t], track[t]->cTG[editpos]);
        region_string(&gn[9], renderTrig[t], 0, 0, 0xf, 0, 0);
    }
    
    //time
    else if (d == 1)
    {
        region_fill(&gn[9], 0x0);
//        print_fix16(renderTime[t], track[t]->cT[editpos]);
        region_string(&gn[9], renderTime[t], 0, 0, 0xf, 0, 0);
    }
    
    //mode
    else if (d == 2)
    {
        region_fill(&gn[9], 0x0);
        render_dstmode(track[t]->m);
    }
    
    //wav
    else if (d == 3)
    {
        region_fill(&gn[9], 0x0);
        region_string(&gn[9], sample_name[track[t]->s[editpos]-8], 0, 0, 0xf, 0, 0);
    }
    
    //offsetA
    else if (d == 4)
    {
        region_fill(&gn[9], 0x0);
    }
    
    //offsetB
    else if (d == 5)
    {
        region_fill(&gn[9], 0x0);
    }
    
    //level
    else if (d == 6)
    {
        region_fill(&gn[9], 0x0);
    }
    
    //frequency
    else if (d == 7)
    {
        region_fill(&gn[9], 0x0);
    }
    
    else ;
}

void render_gscounters(void) {
    region_string(&gn[0], renderGenPos, 0, 0, 0xf, 0, 0);
    region_string(&gn[11], renderEditPos, 0, 0, 0xf, 0, 0);
}
