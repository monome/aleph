#include "generator.h"


//static function declarations
static prgmGenptr alloc_gen(void);
static void env_gen_init(genEnv *e, wave_data data, u32 sr);

static void env_set_waveform(genEnv *e, fract32 wf);
static void env_calc_inc(genEnv *e);
static void env_set_tripPoint(genEnv *e, fract32 trip);
static void env_sync_trig(genEnv *e);
static inline fract32 freq_to_phase(fix16 f);
static fract32 env_lookup(genEnv *e);
static void env_advance(genEnv *e);

//static variables
u8 synctrig;
u8 state;

static const fract32 wavetable[WAVE_SIZE] = {
#include "halfsine_1024.inc"
};

//static function definitions
prgmGenptr alloc_gen(void) {
    return(prgmGenptr)alloc_mem(sizeof(prgmGen));
}


//external function definitions
void gen_init() {
    u8 i;
    gen = alloc_gen();
    
    for(i=0;i<N_GEN;i++)
    {
        env_gen_init(&(gen->env[i]), &wavetable, 48000);
        gen->gD[i] = 0;
        
        gen->gDen[i] = 1;
        gen->gDur[i] = 1;
    }
    
    //  global parameters
    trackpos = 0;
    genpos = 0;
    dstpos = 0;
}

void env_gen_init(genEnv *e, wave_data data, u32 sr) {
    e->data = data;
    slew_init(e->incSlew, 0, 0, 0);
    
    e->output = 0;
    e->phase = 0;
    e->trip = 0;
    e->tripPoint = &(e->phase);
    e->f = 0;
    e->wave = 0;
    e->att = FR32_MAX;
}

void env_set_waveform(genEnv *e, fract32 wf) {
    //    e->shapeSlew.x = wf;
}

void env_set_f(genEnv *e, fix16 f) {
    e->f = f;
    env_calc_inc(e);
}

void env_set_trig(u8 t) {
    synctrig = t;
}

void env_set_tripPoint(genEnv *e, fract32 trip) {
    e->trip = trip;
}

void env_sync_trig(genEnv *e) {
    if(synctrig == 0)
        state = 0;
    else if(state == 0)
    {
        state = 1;
        
        *e->tripPoint &= 0x80000000;
        *e->tripPoint = add_fr1x32((int)e->trip, 0x80000000) & 0x7fffffff;
    }
    
    else if(state == 1)
        ;
}

fract32 freq_to_phase(fix16 f) {
    return add_fr1x32(
                      //  int part
                      (fract32)(((int)(f >> 16) * (int)(WAVE_IPS_NORM))),
                      //  fract part
                      mult_fr1x32x32((f & 0xffff) >> 1, (fract32)(WAVE_IPS_NORM))
                      );
}

void env_calc_inc(genEnv *e) {
    e->incSlew.x = freq_to_phase(fix16_mul(FIX16_ONE, e->f));
}

static inline fract32 table_lookup_idx_mask(fract32 *tab, u32 size, fix16 idx) {
    //  packed arguments
    u32 ia = idx >> 16;
    fract32 x = tab[ia];
    //  truncate arguments to 16 bits
/*
    if (idx < 0)
    {
    idx = (phase + 0x40000000) >> 1;
    }
    else
    {
    idx  = (phase >> 1) + 0x3fffffff;
    }
    return add_fr1x32(x,
                      mult_fr1x32x32(
                                  trunc_fr1x32(sub_fr1x32(tab[(ia + 1) & size], x)),
                                  (idx & 0xffff) >> 1
                                  )
                      );
*/
}

static inline fract32 env_lookup(genEnv *e) {
    fix16 idx = e->phase >> 5;
    fract32 xmul = table_lookup_idx_mask(&(e->wave), WAVE_SIZE, idx);
    return mult_fr1x32x32(e->wave, xmul);
}

fract32 env_next(genEnv *e) {
    slew32_calc(e->incSlew);
    e->inc = e->incSlew.y;
    env_advance(e);
    env_sync_trig(e);
    return env_lookup(e);
}

void env_advance(genEnv *e) {
    e->phase = (((int)e->phase) + ((int)(e->inc))) & 0x7fffffff;
}
