// bfin
#include <fract2float_conv.h>
#include "fract_math.h"

// aleph/dsp
#include "interpolate.h"
#include "table.h"

#include "osc.h"

//----------------
//--- static vars

/// assume all oscs have the same samplerate
/// phase increment at 1hz:
static fix16 ips;

#ifdef OSC_SHAPE_LIMIT
/// phase increment limits
static fix16 incMin;
static fix16 incMax;
static u32 incRange;
// multiplier to map w-> max shape
static u32 shapeLimMul;
#endif

//------------------
//---- static functions


// calculate modulated and bandlimited waveshape
static inline void osc_calc_wm(osc* osc) {
  fract32 sm; // mod shape
  // fract32 sl; // shape limit given current freq

  // add modulation
  //// FIXME: this is dumb, should be multiplied?
  sm = add_fr1x32(osc->shape, mult_fr1x32x32(osc->wmIn, osc->wmAmt) );
  
  //- hacky magic formula for pseudo-bandlimiting:
  //- with maximal bandlimiting, want to limit shape to a function of unit freq
  //- max freq = min shape, min freq = max shape
  // :
  // map phase increment to [0,1] fract32

  /* sl = (fract32)(((u32)(osc->inc) - incRange) * shapeLimMul); */
  /* // invert [0,1] to [1,0] */
  /* sl = sub_fr1x32(FR32_MAX, sl); */
  
  /* // limit */
  /* if(sl < sm) { */
  /*   sm = dsp_lerp32(sm, sl, osc->bandLim); */
  /* } */

  /*
  // ok, time for serious bullshit!
  sm = sub_fr1x32(sm, 
		  mult_fr1x32x32( (fract32)(fix16_sub(osc->inc, incMin) * shapeLimMul),
				   osc->bandLim 
				  )
		  );
  if(sm < 0) { sm = 0; }
  osc->shapeMod = sm;
  */
}

// calculate phase incremnet
static inline void osc_calc_inc( osc* osc) {
  filter_1p_lo_in( &(osc->lpInc), fix16_mul(osc->ratio, fix16_mul(osc->hz, ips)) );

  /// TEST:
  //  osc->inc = fix16_mul(osc->ratio, fix16_mul(osc->hz, ips));
 
}

// calculate phase
static inline void osc_calc_pm(osc* osc) {
  osc->idxMod = fix16_add( osc->idx, 
			   fix16_mul( FRACT_FIX16( mult_fr1x32x32( osc->pmIn, 
								   osc->pmAmt ) ),
				      WAVE_TAB_MAX16
				      ) );
  // wrap negative
  while (BIT_SIGN_32(osc->idxMod)) {
    osc->idxMod = fix16_add(osc->idxMod, WAVE_TAB_MAX16);
  }

  // wrap positive
  while(osc->idxMod > WAVE_TAB_MAX16) { 
    osc->idxMod = fix16_sub(osc->idxMod, WAVE_TAB_MAX16); 
  }
}

// lookup 
static inline fract32 osc_lookup(osc* osc) {
  u32 idxA = osc->shapeMod >> WAVE_TAB_RSHIFT;
  u32 idxB = idxA + 1;
  
  fract32 mul = (osc->shapeMod & WAVE_SHAPE_MASK) << WAVE_TAB_LSHIFT;
  fract32 mulInv = sub_fr1x32(FR32_MAX, mul);
  
  return add_fr1x32( 
		    mult_fr1x32x32(table_lookup_idx_mask( (fract32*)(*(osc->tab))[idxA], 
						     WAVE_TAB_SIZE_1, 
						     osc->idxMod
						     ), mulInv ),
		    mult_fr1x32x32(table_lookup_idx_mask( (fract32*)(*(osc->tab))[idxB],
						     WAVE_TAB_SIZE_1,
						     osc->idxMod 
						     ), mul 
				   ) );
}

// advance phase
static inline void osc_advance(osc* osc) {
  osc->idx = fix16_add(osc->idx, osc->inc);
  while(osc->idx > WAVE_TAB_MAX16) { 
    osc->idx = fix16_sub(osc->idx, WAVE_TAB_MAX16);
  }
}

//----------------
//--- extern funcs

// initialize given table data and samplerate
void osc_init(osc* osc, wavtab_t tab, u32 sr) {
  osc->tab = tab;

  ips = fix16_from_float( (f32)WAVE_TAB_SIZE / (f32)sr );

#ifdef OSC_SHAPE_LIMIT
  incMin = fix16_mul(ips, OSC_HZ_MIN);
  incMax = fix16_mul(ips, OSC_HZ_MAX);
  incRange = (u32)incMax - (u32)incMin;
  shapeLimMul = 0x7fffffff / incRange;
#endif

  filter_1p_lo_init( &(osc->lpInc) , FIX16_ONE);
  filter_1p_lo_init( &(osc->lpShape) , FIX16_ONE);
  filter_1p_lo_init( &(osc->lpPm) , FIX16_ONE);
  filter_1p_lo_init( &(osc->lpWm) , FIX16_ONE);

  osc->val = 0;
  osc->idx = 0;
  osc->ratio = FIX16_ONE;
  osc->shape = 0;
  osc->shapeMod = 0;
  osc->idx = 0;
  osc->idxMod = 0;
  osc->bandLim = FR32_MAX >> 2;
  osc->pmAmt = 0;
  osc->wmAmt = 0;

}

// set waveshape (table)
void osc_set_shape(osc* osc, fract32 shape) {
  filter_1p_lo_in( &(osc->lpShape), shape );
}

// set base frequency in hz
void osc_set_hz(osc* osc, fix16 hz) {
  osc->hz = hz;
  osc_calc_inc(osc);
}

// set fine-tuning ratio
void osc_set_tune(osc* osc, fix16 ratio) {
  osc->ratio = ratio;
  osc_calc_inc(osc);
}

// phase modulation amount
void osc_set_pm(osc* osc, fract32 amt) {
  filter_1p_lo_in( &(osc->lpPm), amt);
}

// shape modulation amount
void osc_set_wm(osc* osc, fract32 amt) {
  filter_1p_lo_in( &(osc->lpWm), amt);
}

// phase modulation input
void osc_pm_in(osc* osc, fract32 val) {
  osc->pmIn = val;
}

// shape modulation input
void osc_wm_in(osc* osc, fract32 val) {
  osc->wmIn = val;
}


// set bandlimiting
void osc_set_bl(osc* osc, fract32 bl) {
  osc->bandLim = bl;
}

// get next frame value
fract32 osc_next(osc* osc) {

  /// update param smoothers
    
  osc->inc = filter_1p_lo_next( &(osc->lpInc) );
  osc->shape = filter_1p_lo_next( &(osc->lpShape) );
  osc->pmAmt = filter_1p_lo_next( &(osc->lpPm) );
  //  osc->wmAmt = filter_1p_lo_next( &(osc->lpWm) );
  

  // calculate waveshape modulation + bandlimiting
  //  osc_calc_wm(osc);
  // doesn't sound great yet anyways
  osc->shapeMod = osc->shape;

  // calculate phase modulation
  osc_calc_pm(osc);

  // advance phase
  osc_advance(osc);
  
  // lookup 
  return osc_lookup(osc);
}
