#include "bl_osc.c"


// set waveshape (table)
extern void blosc_set_shape(blOsc* osc, fract32 shape) {
}

// set frequency
extern void blosc_set_freq(blOsc* osc, fract32 freq) {
}

// set modulated phase
extern void blosc_set_pm(blOsc* osc) {
}

// get next value
extern void blosc_get_next(blOsc* osc) {
  // modulated phase
  fix16 phaseMod;
  // phase of inverted signal
  fix16 phaseInv;
  // main signal
  fract32 sig;
  // inverted signal
  fract32 sigInv;

  // main phase, with modulation
  phase = fix16_add(osc->phase + osc->modPhase);
  // wrap sum
  while(phase > osc->maxIdx) {
    phase = fix16_sub(phase, osc->maxIdx);
  }  

  // main signal
  sig = blosc_lookup(osc, phase);
  //  phase for inverted signal
  phaseInv = fix16_add(phase, osc->invPhase);
  // inverted signal
  sigInv = mult_fr1x32x32(BIT_INVERT_32(blosc_lookup(osc, phase)), osc->invAmp);
  sig = add_fr1x32(sig, sigInv);

  // advance main phase
  osc->phase = fix16_add(osc->phase, osc->inc);
  // wrap after advance
  while(osc->phase > osc->maxIdx) {
    osc->phase = fix16_sub(osc->phase, osc->maxIdx);
  }
  // return signal
  return sig;
}
