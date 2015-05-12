//render.c
//aleph-prgm-avr32
/* region, string, offset x, offset y, color text, color background, size */

#include "render.h"

static void region_update(region *r);

//regions
static region bootScrollRegion = {.w = 128, .h = 64, .x = 0, .y = 0};
static scroll bootScroll;

static region lineRegion = {.w=128, .h=8, .x = 0, .y = 0};

static region sequencerRegion = {.w = 128, .h = 56, .x = 0, .y = 0};
static scroll seqScroll;

static const u8 numRegions_trk = 12;
static const u8 numRegions_mix = 6;
static const u8 numRegions_seq = 4;
static const u8 numRegions_cut = 10;

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

static region cut[10] = {
    {.w=64, .h=8, .x = 0, .y = 0},      //0 "offset"
    {.w=64, .h=8, .x = 64, .y = 0},     //1 "loop"
    {.w=64, .h=16, .x = 0, .y = 8},     //2 offset_cut
    {.w=64, .h=8, .x = 64, .y = 8},     //3 loop_cut
    {.w=64, .h=8, .x = 64, .y = 16},    //4 length
    
    {.w=64, .h=8, .x = 0, .y =  24},    //5 "pos"
    {.w=64, .h=8, .x = 64, .y = 24},    //6 sample position
    {.w=64, .h=32, .x = 0, .y =  32},   //7 "sample"
    {.w=64, .h=24, .x = 64, .y = 32},   //8 sample name
    
    {.w=64, .h=8, .x = 64, .y = 56},    //9 app version
};

static region *cutRegions[10] = {
    &(cut[0]),
    &(cut[1]),
    &(cut[2]),
    &(cut[3]),
    &(cut[4]),
    &(cut[5]),
    &(cut[6]),
    &(cut[7]),
    &(cut[8]),
    &(cut[9]),
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
    for(i = 0; i<numRegions_cut; i++) {
        region_alloc((region*)(cutRegions[i]));
    }
}

void render_startup (void) {
    trkpage = N_TRACKS_1;
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
        if(*p > 0x1)
        {
            *p = 0x1;
        }
        p++;
    }
    scroll_string_front(&bootScroll, (char*)str);
    region_update(bootScroll.reg);
}

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
    
    for(i = 0; i<numRegions_cut; i++) {
        r = cutRegions[i];
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
static void render_amp_envelope(u8 region, s32 env);

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
        "trig",             //1
        "loop",             //2
        "[amp]",            //3
        "[dly]",            //4
        "[thru]"            //5
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
            region_string(&trk[8], "envelope", 0, 0, 0xf, 0, 0);
            render_amp_envelope(9, (track[t]->env - 3));
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

void render_amp_envelope(u8 region, s32 env) {
    static char *envptr[] = {
        "lindec",           //0
        "expodec",          //1
        "Rexpodec",         //2
        "tri",              //3
        "gauss*"            //4        
    };
    
    region_fill(&trk[region], 0x0);
    region_string(&trk[region], envptr[env], 0, 0, 0xf, 0, 0);
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
static void render_env_name(s32 name);

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
    scroll_sequence(trkpage);
    render_seqtrack(trkpage);
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
        if (track[t]->m == 1) appendln(sample_name[track[t]->s[i]-8]);
        if (track[t]->m == 2) appendln(sample_name[track[t]->s[i]-8]);
        if (track[t]->m == 3) render_env_name(track[t]->e[i]);
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
        if (track[t]->m == 1) appendln(sample_name[track[t]->s[i]-8]);
        if (track[t]->m == 2) appendln(sample_name[track[t]->s[i]-8]);
        if (track[t]->m == 3) render_env_name(track[t]->e[i]);
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

void render_env_name(s32 name) {
    static char *nameptr[] = {
        "-",
        "sync T"
    };
    
    appendln(nameptr[name]);
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


//draw cut page
void render_cut(void) {
    u8 i;
    const u8 s = samplepos;
    
    for(i = 0; i<numRegions_cut; i++) {
        region_fill(&cut[i], 0x0);
    }
    
    region_string(&cut[0], "offset", 0, 0, 0xf, 0, 0);    
    region_string(&cut[1], "loop", 0, 0, 0xf, 0, 0);
    
    print_fix16(renderOffsetCut, sample[s]->offset_cut);
    region_string(&cut[2], renderOffsetCut, 0, 0, 0xf, 0, 0);
    
    print_fix16(renderLoopCut, sample[s]->loop_cut);
    region_string(&cut[3], renderLoopCut, 0, 0, 0xf, 0, 0);

    print_fix16(renderSampleLoop, (sample[s]->loop - sample[s]->offset_cut + (sample[s]->loop_cut)));
    region_string(&cut[4], renderSampleLoop, 0, 0, 0xf, 0, 0);

    region_string(&cut[5], "pos", 0, 0, 0xf, 0, 0);
    
    region_fill(&cut[6], 0x0);
    clearln();
    appendln_idx_lj((u8)(sample[s]->num-8));
    endln();
    font_string_region_clip(&cut[6], lineBuf, 0, 0, 0xf, 0);
    
    region_string(&cut[7], "sample", 0, 0, 0xf, 0, 0);
    region_string(&cut[8], sample_name[s-8], 0, 0, 0xf, 0, 0);

    region_string(&cut[9], "     [prgm 0.0.8]", 0, 0, 0x1, 0, 0);
}
