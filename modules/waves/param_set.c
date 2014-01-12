// set param values.
// this is a separate file for convenience only.

void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {

  case eParamFreq1:
    osc_set_hz( &osc1, v );
    break;
  case eParamFreq2:
    osc_set_hz( &osc2, v );
    break;

  case eParamTune1:
    osc_set_tune( &osc1, v );
    break;
  case eParamTune2:
    osc_set_tune( &osc2, v );
    break;

  case eParamWave1:
    osc_set_shape( &osc1, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamWave2:
    osc_set_shape( &osc2, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

  case eParamPm12:
    osc_set_pm ( &osc2, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamPm21:
    osc_set_pm ( &osc1, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

  case eParamWm12:
    osc_set_wm ( &osc2, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamWm21:
    osc_set_wm ( &osc1, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

  case eParamAmp1:
     filter_1p_lo_in(amp1Lp, v);
    break;
  case eParamAmp2:
    filter_1p_lo_in(amp2Lp, v);
    break;

  case eParamBl1 :
    osc_set_bl( &osc1, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamBl2 :
    osc_set_bl( &osc1, BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

    //// filter params:
  case eParam_cut1 :
    filter_svf_set_coeff(&(svf1), v );
    break;
  case eParam_cut2 :
    filter_svf_set_coeff(&(svf2), v );
    break;
  case eParam_rq1 :
    // incoming param value is 16.16
    // target is 2.30
    //    filter_svf_set_rq(&(svf1), v);
    filter_svf_set_rq(&(svf1), v << 14);
    break;
  case eParam_rq2 :
    //    filter_svf_set_rq(&(svf2), v);
    filter_svf_set_rq(&(svf2), v << 14);
    break;
  case eParam_low1 :
    filter_svf_set_low(&(svf1), v);
    break;
  case eParam_low2 :
    filter_svf_set_low(&(svf2), v);
    break;
  case eParam_high1 :
    filter_svf_set_high(&(svf1), v);
    break;
  case eParam_high2 :
    filter_svf_set_high(&(svf2), v);
    break;
  case eParam_band1 :
    filter_svf_set_band(&(svf1), v);
    break;
  case eParam_band2 :
    filter_svf_set_band(&(svf2), v);
    break;
  case eParam_notch1 :
    filter_svf_set_notch(&(svf1), v);
    break;
  case eParam_notch2 :
    filter_svf_set_notch(&(svf2), v);
    break;

    // filter balance
  case eParam_fwet1 :
    fwet1 = v;
    break;

  case eParam_fwet2 :
    fwet2 = v;
    break;

  case eParam_fdry1 :
    fdry1 = v;
    break;

  case eParam_fdry2 :
    fdry2 = v;
    break;

    ///// smoothers

  case eParamFreq1Smooth:
    filter_1p_lo_set_slew( &(osc1.lpInc), v); 
    break;
  case eParamFreq2Smooth:
    filter_1p_lo_set_slew( &(osc2.lpInc), v); 
    break;

  case eParamPm21Smooth:
    filter_1p_lo_set_slew( &(osc1.lpPm), v); 
    break;
  case eParamPm12Smooth:
    filter_1p_lo_set_slew( &(osc2.lpPm), v); 
    break;

  case eParamWm21Smooth:
    filter_1p_lo_set_slew( &(osc1.lpWm), v); 
    break;
  case eParamWm12Smooth:
    filter_1p_lo_set_slew( &(osc2.lpWm), v); 
    break;

  case eParamWave1Smooth:
    filter_1p_lo_set_slew( &(osc1.lpShape), v); 
    break;
  case eParamWave2Smooth:
    filter_1p_lo_set_slew( &(osc2.lpShape), v); 
    break;
  case eParamAmp1Smooth:
    filter_1p_lo_set_slew(amp1Lp, v);
    break;
  case eParamAmp2Smooth:
    filter_1p_lo_set_slew(amp2Lp, v);
    break;

  case eParamIoAmp0:
    ioAmp0 = (v);
    break;
  case eParamIoAmp1:
    ioAmp1 = (v);
    break;
  case eParamIoAmp2:
    ioAmp2 = (v);
    break;
  case eParamIoAmp3:
    ioAmp3 = (v);
    break;

    // dac values
  case eParam_dacVal0 :
    filter_1p_lo_in(&(dacSlew[0]), (v >> ( PARAM_DAC_RADIX - 1))  & DAC_VALUE_MASK);
    //dac_update(0, v >> (PARAM_DAC_RADIX - 1));
    break;
  case eParam_dacVal1 :
    filter_1p_lo_in(&(dacSlew[1]), (v >> (PARAM_DAC_RADIX - 1)) & DAC_VALUE_MASK);
    //dac_update(1, v >> (PARAM_DAC_RADIX - 1));
    break;
  case eParam_dacVal2 :
    filter_1p_lo_in(&(dacSlew[2]), (v >> (PARAM_DAC_RADIX - 1))  & DAC_VALUE_MASK);
    //dac_update(2, v >> (PARAM_DAC_RADIX - 1));
    break;
  case eParam_dacVal3 :
    filter_1p_lo_in(&(dacSlew[3]), (v >> (PARAM_DAC_RADIX - 1))  & DAC_VALUE_MASK);
    //dac_update(3, v >> (PARAM_DAC_RADIX - 1));
    break;

  case eParam_dacSlew0 :
    filter_1p_lo_set_slew(&(dacSlew[0]), v);
    break;
  case eParam_dacSlew1 :
    filter_1p_lo_set_slew(&(dacSlew[1]), v);
    break;
  case eParam_dacSlew2 :
    filter_1p_lo_set_slew(&(dacSlew[2]), v);
    break;
  case eParam_dacSlew3 :
    filter_1p_lo_set_slew(&(dacSlew[3]), v);
    break;
  default:
    break;
  }

}
