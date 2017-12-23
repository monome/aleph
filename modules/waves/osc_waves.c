// bfin
#include <fract2float_conv.h>
#include "fract_math.h"
#include "libfixmath/fix16_fract.h"

// aleph/dsp
#include "interpolate.h"
#include "table.h"

#include "osc_waves.h"
#include "osc_polyblep.h"
#include "slew.h"

//----------------
//--- static vars

/// assume all oscs have the same samplerate
/// phase increment at 1hz:
//static fix16 ips;

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

// freq_to_phase in 2.30
#define FREQ_TO_PHASE_CONST 0x57619F00

// convert fix16 frequency to normalized fract32 phase increment
static inline fract32 freq_to_phase(fix16 freq) {
  u8 rad = norm_fr1x32(freq);
  freq = shl_fr1x32(freq, rad);
  fract32 ret = mult_fr1x32x32(freq, FREQ_TO_PHASE_CONST);
  return shr_fr1x32(ret, rad - 1);
}

  
// calculate modulated and bandlimited waveshape
static inline void osc_calc_wm(osc* osc) {
  // FIXME: attempt better bandlimiting again
#if 1 // ???
  osc->shapeMod = abs_fr1x16(add_fr1x16(osc->shapeSlew.y,
					mult_fr1x16(trunc_fr1x32(osc->wmIn), 
						    osc->wmSlew.y)));
#else
  // shapeMod =shape + (wmIn*0.5+0.5) * wmAmt
  osc->shapeMod = add_fr1x16(osc->wmIn >> 1, FR16_MAX >> 1);
  osc->shapeMod = multr_fr1x16(osc->shapeMod, osc->wmSlew.y);
  osc->shapeMod = add_fr1x16(osc->shapeMod, osc->shapeSlew.y);
  if(osc->shapeMod < 0) {
    osc->shapeMod = 0;
  }
  /* osc->shapeMod = FR16_MAX >> 3; */
  /* osc->shapeMod = osc->shapeSlew.y; */
#endif
}

// calculate phase incremnet
static inline void osc_calc_inc( osc* osc) {
  osc->incSlew.x = freq_to_phase( fix16_mul_fract(osc->ratio, osc->hz) ); 
}

// calculate phase
static inline void osc_calc_pm(osc* osc) {
  // non-saturated add, allow overflow, zap sign
  osc->phaseMod = osc->phase + mult_fr1x32(trunc_fr1x32(osc->pmIn), osc->pmSlew.y);
}

// lookup 
static inline fract32 osc_lookup(osc* osc) {
  // index of wavetables and interpolation weights for shape
  u32 waveIdxA = osc->shapeMod >> (WAVE_SHAPE_IDX_SHIFT);
  u32 waveIdxB = waveIdxA + 1; // no bounds check !
  fract16 waveMulB = (osc->shapeMod & (WAVE_SHAPE_MASK)) << (WAVE_SHAPE_MUL_SHIFT);
  fract16 waveMulA = sub_fr1x16(0x7fff, waveMulB); 

  fract32 pMod_unsigned = ((u32)osc->phaseMod) >> 1;
  
  //  signal index and interpolation weights for both wavetables
  u32 signalIdxA = pMod_unsigned >> WAVE_IDX_SHIFT; 
  u32 signalIdxB = (signalIdxA + 1) & WAVE_TAB_SIZE_1; // need the check here
  fract16 signalMulB = (fract16)((pMod_unsigned & (WAVE_IDX_MASK)) >> (WAVE_IDX_MUL_SHIFT));
  fract16 signalMulA = sub_fr1x16(0x7fff, signalMulB); 
  

  return add_fr1x32(
		    // table A
		    mult_fr1x32(
				trunc_fr1x32(
					     add_fr1x32(
							// signal A, scaled
							mult_fr1x32( 
								    trunc_fr1x32( (*(osc->tab))[waveIdxA][signalIdxA] ),
								    signalMulA 
								     ),
							// signal B, scaled
							mult_fr1x32( 
								    trunc_fr1x32( (*(osc->tab))[waveIdxA][signalIdxB] ),
								    signalMulB
								     )
							)
					     ),
				waveMulA
				),
		    // table B
		    mult_fr1x32(
				trunc_fr1x32(
					     add_fr1x32(
							// signal A, scaled
							mult_fr1x32( 
								    trunc_fr1x32( (*(osc->tab))[waveIdxB][signalIdxA] ),
								    signalMulA 
								     ),
							// signal B, scaled
							mult_fr1x32( 
								    trunc_fr1x32( (*(osc->tab))[waveIdxB][signalIdxB] ),
								    signalMulB
								     )
							)
					     ),
				waveMulB
				)
		    );

  
  /* return add_fr1x32(  */
  /* 		    mult_fr1x32x32(table_lookup_idx_mask( (fract32*)(*(osc->tab))[idxA],  */
  /* 						     WAVE_TAB_SIZE_1,  */
  /* 						     osc->idxMod */
  /* 						     ), mulInv ), */
  /* 		    mult_fr1x32x32(table_lookup_idx_mask( (fract32*)(*(osc->tab))[idxB], */
  /* 						     WAVE_TAB_SIZE_1, */
  /* 						     osc->idxMod  */
  /* 						     ), mul  */
  /* 				   ) ); */
}

// alternative method no lookup tables
static inline fract32 osc_polyblep(osc* osc) {
  //  signal index and interpolation weights for both waveshapes
  fract16 mulB = osc->shapeMod;
  fract16 mulA = sub_fr1x16(FR16_MAX, mulB);
  fract16 sigA, sigB;
  sigA = sine_polyblep(osc->phaseMod);
  sigB = saw_polyblep(osc->phaseMod, osc->incSlew.y);
  return add_fr1x32(mult_fr1x32(sigA, mulA),
		    mult_fr1x32(sigB, mulB));
}

// advance phase
static inline void osc_advance(osc* osc) {
  // phase is normalized fract32 in [-1, 1)
  // use non-saturating add, allow overflow
  osc->phase = osc->phase + osc->incSlew.y;
}

//----------------
//--- extern funcs

// initialize given table data and samplerate
void osc_init(osc* osc, wavtab_t tab, u32 sr) {
  osc->tab = tab;

  //  ips = fix16_from_float( (f32)WAVE_TAB_SIZE / (f32)sr );

#ifdef OSC_SHAPE_LIMIT
  /* incMin = fix16_mul_fract(ips, OSC_HZ_MIN); */
  /* incMax = fix16_mul_fract(ips, OSC_HZ_MAX); */
  /* incRange = (u32)incMax - (u32)incMin; */
  /* shapeLimMul = 0x7fffffff / incRange; */
#endif
  slew_init(osc->incSlew, 0, 0, 0 );
  slew_init(osc->shapeSlew, 0, 0, 0 );
  slew_init(osc->pmSlew, 0, 0, 0 );
  slew_init(osc->wmSlew, 0, 0, 0 );

  osc->phase = 0;
  osc->ratio = FIX16_ONE;
  osc->hz = FIX16_ONE;

}

// set waveshape (table)
void osc_set_shape(osc* osc, fract16 shape) {
  //  filter_1p_lo_in( &(osc->lpShape), shape );
  osc->shapeSlew.x = shape;
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
void osc_set_pm(osc* osc, fract16 amt) {
  osc->pmSlew.x = amt;
}

// shape modulation amount
void osc_set_wm(osc* osc, fract16 amt) {
  osc->wmSlew.x = amt;
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
/* void osc_set_bl(osc* osc, fract32 bl) { */
/*   osc->bandLim = bl; */
/* } */

// get next frame value
fract32 osc_next(osc* osc) {

  /// update param smoothers
  slew16_calc ( osc->pmSlew );
  slew16_calc ( osc->wmSlew );
  /* slew16_calc ( osc->shapeSlew ); */
  osc->shapeSlew.y = osc->shapeSlew.x;
  slew32_calc ( osc->incSlew);

  /* osc->inc = osc->incSlew.y; */
  /* osc->shape = osc->shapeSlew.y; */

  /// FIXME:
  // shape mod doesn't sound awesome right now anyways
  // add mix points, then think about it

  // calculate waveshape modulation + bandlimiting
  osc_calc_wm(osc);
  //  osc->shapeMod = osc->shape << 16;

  // calculate phase modulation
  osc_calc_pm(osc);

  // advance phase
  osc_advance(osc);
  
  // lookup 
  return osc_lookup(osc);
}
u8 svf_mode[2] = {0, 0};
