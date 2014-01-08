/* mono.c
 * aleph-bfin
 * 
 * PM chain synthesizer, 2 oscs, on all outputs,
 * plus i/o loop on each i/o pair
 */

// std
#include <math.h>
#include <stdlib.h>
#include <string.h>
// aleph-common
#include "fix.h"
// audio lib
#include "filter_1p.h"
#include "filter_svf.h"
#include "table.h"
#include "conversion.h"
// bfin
#include "bfin_core.h"
#include "fract_math.h"
#include <fract2float_conv.h>

#include "module.h"
#include "params.h"
#include "types.h"


//-------- data types


// define a local data structure that subclasses moduleData.
// use this for all data that is large and/or not speed-critical.
// this structure should statically allocate all necessary memory 
// so it can simply be loaded at the start of SDRAM.
typedef struct _monoFmData {
  moduleData super;
  ParamDesc mParamDesc[eParamNumParams];
  ParamData mParamData[eParamNumParams];
} monoFmData;

//--- wavetables
// how many wavetables
#define WAVE_TAB_NUM 5
#define WAVE_TAB_SIZE 	1024
// rshift from shape variable to get table index
#define WAVE_TAB_RSHIFT 29
// mask to get interpolation constant
#define WAVE_TAB_MASK 0x1fffffff
// lshift after mask to get multiplier 
#define WAVE_TAB_LSHIFT 2

#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)
#define WAVE_TAB_MAX16 	(WAVE_TAB_SIZE * FIX16_ONE - 1)

//-------------------------
//----- extern vars (initialized here)
moduleData * gModuleData; // module data

//-----------------------
//------ static variables

// pointer to local module data, initialize/v at top of SDRAM
static monoFmData * monoData;

//-- static allocation (SRAM) for variables that are small and/or frequently accessed:

/* static   fract32   tab1[WAVE_TAB_SIZE]; // wavetable1 */
/* static   fract32   tab2[WAVE_TAB_SIZE]; // wavetable1 */

/// FIXME: wavetables are statically linked constants for now.
/// would like to have them in SDRAM and allow arbitrary asynchronous load.
static const fract32 wavtab[WAVE_TAB_NUM][WAVE_TAB_SIZE] = { 
#include "wavtab_data_inc.c" 
};


static u32       sr;            // sample rate
static fix16    ips;        // index change per sample

static fix16     idx1;          // current phase (fractional idx) (primary)
static fix16     idx2;          // current phase (fractional idx) (secondary)
static fix16     idx1Mod;       // modulated primary index
static fix16     inc1;          // idx change at current frequency (primary)
static fix16     inc2;          // idx change at current frequency (secondary)
static fract32   osc1;          // primary oscillator output
static fract32   osc2;          // secondary oscillator output
static fract32   frameVal;      // output value
static fract32   pm;           // phase modulation depth

static fix16     freq1;            // base frequency (primary oscillator)
static fix16     freq2;            // base frequency (secondary oscillator)
static fix16     ratio2;         // frequency ratio for secondary oscillator
//static u8        track;         // pitch-tracking flag

static fract32   wave1;          // waveshape (primary)
static fract32   wave2;          // waveshape (secondary)

static fract32   amp1;          // amplitude (primary)
static fract32   amp2;          // amplitude (secondary)

// filters
static filter_svf svf1;
static filter_svf svf2;

// this is kind of unnecessary really. amp integrators do a better job...
// static env_asr*  env;           // ASR amplitude envelope
 
static filter_1p_lo* freq1Lp;  // 1plp smoother for freq1
static filter_1p_lo* freq2Lp;  // 1plp smoother for freq2
static filter_1p_lo* pmLp;    // 1plp smoother for pm
static filter_1p_lo* wave1Lp; // 1plp smoother for wave1
static filter_1p_lo* wave2Lp; // 1plp smoother for wave2
static filter_1p_lo* amp1Lp;  // 1plp smoother for amp1
static filter_1p_lo* amp2Lp;  // 1plp smoother for amp2

// dry I->O amplitudes
static fract32 ioAmp0;
static fract32 ioAmp1;
static fract32 ioAmp2;
static fract32 ioAmp3;

//----------------------
//----- static function declaration
// set primary freq
static inline void set_freq1(fix16 freq);
// set secondary pitch ratio - enters pitch tracking mode
//static inline void set_ratio2(const fix16 ratio);
// set secondary frequency - leaves pitch tracking mode
static inline void set_freq2(fix16 freq);
// double-lookup and interpolate
static inline fract32 lookup_wave(const fix16 idx, const fract32 wave);
// frame calculation
static void calc_frame(void);
// set all parameter descriptors
//static void fill_param_desc(void);

//----------------------
//----- static functions

// set primary freq
static inline void set_freq1(fix16 freq) {  
  if( freq < OSC_FREQ_MIN ) freq = OSC_FREQ_MIN;
  if( freq > OSC_FREQ_MAX ) freq = OSC_FREQ_MAX;
  filter_1p_lo_in(freq1Lp, freq);
  freq1 = freq;
  /* if(track) { */
  /*   filter_1p_lo_in(freq2Lp, fix16_mul(freq1, ratio2) ) ; */
  /* } */
}

/* // set secondary pitch ratio - enters pitch tracking mode */
/* static inline void set_ratio2(const fix16 ratio) { */
/*   ratio2 = ratio; */
/*   if (ratio2 > RATIO_MAX) ratio2 = RATIO_MAX; */
/*   if (ratio2 < RATIO_MIN) ratio2 = RATIO_MIN; */
/*   filter_1p_lo_in(freq2Lp, fix16_mul(freq1, ratio2)); */
/*   //  track = 1; */
/* } */

// set secondary frequency - leaves pitch tracking mode
static inline void set_freq2(fix16 freq) {
  if( freq < OSC_FREQ_MIN ) freq = OSC_FREQ_MIN;
  if( freq > OSC_FREQ_MAX ) freq = OSC_FREQ_MAX;
  filter_1p_lo_in(freq2Lp, freq);
  //  track = 0;
}

// double-lookup and interpolate
static inline fract32 lookup_wave(const fix16 idx, const fract32 wave) {

  // assume wave variable is positive
  // shift shape param right to get table index


#if 1
  u32 idxA = wave >> WAVE_TAB_RSHIFT;
  u32 idxB = idxA + 1;

  fract32 mul = (wave & WAVE_TAB_MASK) << WAVE_TAB_LSHIFT;
  fract32 mulInv = sub_fr1x32(FR32_MAX, mul);
#else   /// TEST
  /* u32 idxA = 0; */
  /* u32 idxB = idxA + 1; */

  /* fract32 mul = wave; */
  /* fract32 mulInv = sub_fr1x32(FR32_MAX, mul); */
#endif



  return add_fr1x32( 
		    mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxA], WAVE_TAB_SIZE, idx), mulInv),
		    mult_fr1x32x32(table_lookup_idx((fract32*)wavtab[idxB], WAVE_TAB_SIZE, idx), mul)
		     );

  ///// FIXME: this is far from optimized.
  /* const fract32 waveInv = sub_fr1x32(FR32_MAX, wave); */
  /* return add_fr1x32(  */
  /* 		    mult_fr1x32x32(table_lookup_idx(tab1, WAVE_TAB_SIZE, idx), waveInv), */
  /* 		    mult_fr1x32x32(table_lookup_idx(tab2, WAVE_TAB_SIZE, idx), wave) */
  /* 		     ); */
}

// frame calculation
static void calc_frame(void) {

  /// testing
  fract32 tmp;

  // ----- smoothers:
  // pm
  pm = filter_1p_lo_next(pmLp);

  // amp1
  amp1 = filter_1p_lo_next(amp1Lp);
  // amp2
  amp2 = filter_1p_lo_next(amp2Lp);

  // wave1
  wave1 = filter_1p_lo_next(wave1Lp);
  // wave2
  wave2 = filter_1p_lo_next(wave2Lp);

  // lookup osc2
  osc2 = lookup_wave(idx2, wave2);
  
  /// use osc2 output as phase modulation, scaled to tablesize
  idx1Mod = fix16_add(
		      idx1,
		      fix16_mul(
				FRACT_FIX16(mult_fr1x32x32(osc2, pm)),
				WAVE_TAB_MAX16)
		      );

  // wrap negative
  while (BIT_SIGN_32(idx1Mod)) {
    idx1Mod = fix16_add(idx1Mod, WAVE_TAB_MAX16);
  }

  // wrap positive
  while(idx1Mod > WAVE_TAB_MAX16) { 
    idx1Mod = fix16_sub(idx1Mod, WAVE_TAB_MAX16); 
  }

  // lookup osc11
  osc1 = lookup_wave(idx1Mod, wave1);


  ///////////
  ///////////
  // apply filters
  tmp = filter_svf_next( &(svf1), osc1);  
  tmp = filter_svf_next( &(svf2), osc2);  
  /////////
  /////////


  // apply amplitudes and sum 
  frameVal = add_fr1x32(
			mult_fr1x32x32(osc1, amp1),
			mult_fr1x32x32(osc2, amp2)
			);


  /* // increment and apply envelope */
  /* frameVal = mult_fr1x32x32(frameVal, env_asr_next(env)); */

  // increment and apply freq smoothers
  if(!(freq1Lp->sync)) {
    inc1 = fix16_mul(filter_1p_lo_next(freq1Lp), ips);
    //    set_freq
  } 
  if(!(freq2Lp->sync)) {
    inc2 = fix16_mul(filter_1p_lo_next(freq2Lp), ips);
  } 

  // increment phasor idx and wrap
  idx1 = fix16_add(idx1, inc1);
  while(idx1 > WAVE_TAB_MAX16) { 
    idx1 = fix16_sub(idx1, WAVE_TAB_MAX16);
  }

  idx2 = fix16_add(idx2, inc2);
  while(idx2 > WAVE_TAB_MAX16) { 
    idx2 = fix16_sub(idx2, WAVE_TAB_MAX16); 
  }
}

//----------------------
//----- external functions

void module_init(void) {
  u32 i;

  // init module/param descriptor
  // intialize local data at start of SDRAM
  monoData = (monoFmData * )SDRAM_ADDRESS;
  // initialize moduleData superclass for core routines
  gModuleData = &(monoData->super);
  strcpy(gModuleData->name, "aleph-mono");
  gModuleData->paramDesc = monoData->mParamDesc;
  gModuleData->paramData = monoData->mParamData;
  gModuleData->numParams = eParamNumParams;

  fill_param_desc();

  // init params

  /// fill values with minima as default
  for(i=0; i<eParamNumParams; ++i) {
    gModuleData->paramData[i].value = gModuleData->paramDesc[i].min;
  }

  sr = SAMPLERATE;
  ips = fix16_from_float( (f32)WAVE_TAB_SIZE / (f32)sr );

  //track = 1;
  amp1 = amp2 = INT32_MAX >> 2;
  
  freq1 = fix16_from_int(220);
  freq2 = fix16_from_int(330);

  ratio2 = fix16_from_float(1.5);
  idx1 = idx2 = 0;

  ioAmp0 = FR32_MAX;
  ioAmp1 = FR32_MAX;
  ioAmp2 = FR32_MAX;
  ioAmp3 = FR32_MAX;

  // non-default initial values 
  // (FIXME: defaults should just be more correct i guess?)
 
  /// ok, for now
  gModuleData->paramData[eParamFreq1 ].value = freq1 ;
  gModuleData->paramData[eParamFreq2 ].value = freq2 ;
  //  gModuleData->paramData[eParamRatio2 ].value = ratio2 ;

  /// FIXME: silly way to get amplitudes back.
  // this stuff will get better with unified param scaling.
  gModuleData->paramData[eParamAmp1].value = amp1;
  gModuleData->paramData[eParamAmp2].value = amp2;

  gModuleData->paramData[eParamIoAmp0 ].value = ioAmp0;
  gModuleData->paramData[eParamIoAmp1 ].value = ioAmp1;
  gModuleData->paramData[eParamIoAmp2 ].value = ioAmp2;
  gModuleData->paramData[eParamIoAmp3 ].value = ioAmp3;


  // init wavetables
  //  table_fill_harm(tab1, WAVE_TAB_SIZE, 1, 1.f, 0);
  //  table_fill_harm(tab2, WAVE_TAB_SIZE, 5, 0.5f, 1);

  // allocate envelope
  /* env = (env_asr*)malloc(sizeof(env_asr)); */
  /* env_asr_init(env); */

  /* env_asr_set_atk_shape(env, float_to_fr32(0.5)); */
  /* env_asr_set_rel_shape(env, float_to_fr32(0.5)); */
  /* env_asr_set_atk_dur(env, 1000); */
  /* env_asr_set_rel_dur(env, 10000); */

  // filters
  filter_svf_init(&(svf1));
  filter_svf_init(&(svf2));    

  filter_svf_set_rq(&(svf1), 0x1000);
  filter_svf_set_low(&(svf1), 0x4000);
  filter_svf_set_coeff(&(svf1), 0x5ff00000 );
    
  filter_svf_set_rq(&(svf2), 0x1000);
  filter_svf_set_low(&(svf2), 0x4000);
  filter_svf_set_coeff(&(svf2), 0x4ff00000 );
    

  // allocate smoothers
  freq1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( freq1Lp, freq1 );

  freq2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( freq2Lp, freq2 );

  pmLp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( pmLp, pm );

  wave1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( wave1Lp, wave1 );

  wave2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( wave2Lp, wave2 );

  amp1Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( amp1Lp, amp1 );

  amp2Lp = (filter_1p_lo*)malloc(sizeof(filter_1p_lo));
  filter_1p_lo_init( amp2Lp, amp2 );

  // initialize osc phasor increment
  inc1 = fix16_mul(freq1, ips);
  inc2 = fix16_mul(freq2, ips);
;
}

// de-init
void module_deinit(void) {
  //  free(env);
  free(freq1Lp);
  free(freq2Lp);
  free(pmLp);
  free(wave1Lp);
  free(wave2Lp);
  free(amp1Lp);
  free(amp2Lp);
}


// get number of parameters
extern u32 module_get_num_params(void) {
  return eParamNumParams;
}

// frame callback
void module_process_frame(void) {
  calc_frame();
  out[0] = add_fr1x32(frameVal, mult_fr1x32x32(in[0], ioAmp0));
  out[1] = add_fr1x32(frameVal, mult_fr1x32x32(in[1], ioAmp1));
  out[2] = add_fr1x32(frameVal, mult_fr1x32x32(in[2], ioAmp2));
  out[3] = add_fr1x32(frameVal, mult_fr1x32x32(in[3], ioAmp3));
}

#include "param_set.c"
