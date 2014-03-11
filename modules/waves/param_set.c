// set param values.
// this is a separate file for convenience only.

void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {

  case eParamHz1:
    osc_set_hz( &(voice[1].osc), v );
    break;
  case eParamHz0:
    osc_set_hz( &(voice[0].osc), v );
    break;

  case eParamTune1:
    osc_set_tune( &(voice[1].osc), v );
    break;
  case eParamTune0:
    osc_set_tune( &(voice[0].osc), v );
    break;

  case eParamWave1:
    osc_set_shape( &(voice[1].osc), BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamWave0:
    osc_set_shape( &(voice[0].osc), BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

    // FIXME: this is assuming simple fixed x-modulation with 2 voices.
    /// should have a matrix of modulation mix points.
  case eParamPm10:
    osc_set_pm ( &(voice[0].osc), BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamPm01:
    osc_set_pm ( &(voice[1].osc), BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

  case eParamWm10:
    osc_set_wm ( &(voice[0].osc), BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamWm01:
    osc_set_wm ( &(voice[1].osc), BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

  case eParamAmp1:
    //     filter_1p_lo_in(amp1Lp, v);
    filter_1p_lo_in(&(voice[1].ampSlew), v);
    
    break;
  case eParamAmp0:
    //    filter_1p_lo_in(amp0Lp, v);
    filter_1p_lo_in(&(voice[0].ampSlew), v);
    break;

  case eParamBl1 :
    //    osc_set_bl( &(voice[1].osc), BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;
  case eParamBl0 :
    //    osc_set_bl( &(voice[0].osc), BIT_ABS_32(FIX16_FRACT_TRUNC(v)) );
    break;

    //// filter params:
  case eParam_cut1 :
    //    filter_svf_set_coeff(&(voice[1].svf), v );
    filter_1p_lo_in( &(voice[1].cutSlew), v );
    break;
  case eParam_cut0 :
    //    filter_svf_set_coeff(&(voice[0].svf), v );
    filter_1p_lo_in( &(voice[0].cutSlew), v );
    break;
  case eParam_rq1 :
    // incoming param value is 16.16
    // target is 2.30
    //    filter_svf_set_rq(&(voice[1].svf), v);
    //    filter_svf_set_rq(&(voice[1].svf), v << 14);
    filter_1p_lo_in( &(voice[1].rqSlew), v << 14 ); 
    break;
  case eParam_rq0 :
    //    filter_svf_set_rq(&(voice[0].svf), v);
    //    filter_svf_set_rq(&(voice[0].svf), v << 14);
    filter_1p_lo_in( &(voice[0].rqSlew), v << 14 );

    break;
  case eParam_low1 :
    filter_svf_set_low(&(voice[1].svf), v);
    break;
  case eParam_low0 :
    filter_svf_set_low(&(voice[0].svf), v);
    break;
  case eParam_high1 :
    filter_svf_set_high(&(voice[1].svf), v);
    break;
  case eParam_high0 :
    filter_svf_set_high(&(voice[0].svf), v);
    break;
  case eParam_band1 :
    filter_svf_set_band(&(voice[1].svf), v);
    break;
  case eParam_band0 :
    filter_svf_set_band(&(voice[0].svf), v);
    break;
  case eParam_notch1 :
    filter_svf_set_notch(&(voice[1].svf), v);
    break;
  case eParam_notch0 :
    filter_svf_set_notch(&(voice[0].svf), v);
    break;

    // filter balance
  case eParam_fwet0 :
    voice[0].fWet = v;
    break;

  case eParam_fwet1 :
    voice[1].fWet = v;
    break;

  case eParam_fdry0 :
    voice[0].fDry = v;
    break;

  case eParam_fdry1 :
    voice[1].fDry = v;
    break;

    ///// slewers

  case eParamHz1Slew:
    //    filter_1p_lo_set_slew( &(voice[1].osc.lpInc), v); 
    voice[1].osc.incSlew.c = v;
    break;
  case eParamHz0Slew:
    //    filter_1p_lo_set_slew( &(voice[0].osc.lpInc), v); 
    voice[0].osc.incSlew.c = v;
    break;

  case eParamPm01Slew:
    //    filter_1p_lo_set_slew( &(voice[1].osc.lpPm), v); 
    voice[1].osc.pmSlew.c = trunc_fr1x32(v);
    break;
  case eParamPm10Slew:
    //    filter_1p_lo_set_slew( &(voice[0].osc.lpPm), v); 
    voice[0].osc.pmSlew.c = trunc_fr1x32(v);
    break;

  case eParamWm01Slew:
    //    filter_1p_lo_set_slew( &(voice[1].osc.lpWm), v); 
    voice[1].osc.wmSlew.c = trunc_fr1x32(v);
    break;
  case eParamWm10Slew:
    //    filter_1p_lo_set_slew( &(voice[0].osc.lpWm), v); 
    voice[0].osc.wmSlew.c = trunc_fr1x32(v);
    break;

  case eParamWave1Slew:
    //    filter_1p_lo_set_slew( &(voice[1].osc.lpShape), v); 
    break;
  case eParamWave0Slew:
    //    filter_1p_lo_set_slew( &(voice[0].osc.lpShape), v); 
    break;
  case eParamAmp1Slew:
    filter_1p_lo_set_slew( &(voice[1].ampSlew), v);
    break;
  case eParamAmp0Slew:
    filter_1p_lo_set_slew( &(voice[0].ampSlew), v);
    break;

    // param integrators
  case eParamCut0Slew :
    filter_1p_lo_set_slew(&(voice[0].cutSlew), v);
    break;
  case eParamCut1Slew :
    filter_1p_lo_set_slew(&(voice[1].cutSlew), v);
    break;

  case eParamRq0Slew :
    filter_1p_lo_set_slew(&(voice[0].rqSlew), v);
    break;
  case eParamRq1Slew :
    filter_1p_lo_set_slew(&(voice[1].rqSlew), v);
    break;

    // cv values
  case eParam_cvVal0 :
    filter_1p_lo_in(&(cvSlew[0]), (v >> ( PARAM_DAC_RADIX - 1))  & DAC_VALUE_MASK);
    //cv_update(0, v >> (PARAM_CV_RADIX - 1));
    break;
  case eParam_cvVal1 :
    filter_1p_lo_in(&(cvSlew[1]), (v >> (PARAM_DAC_RADIX - 1)) & DAC_VALUE_MASK);
    //cv_update(1, v >> (PARAM_CV_RADIX - 1));
    break;
  case eParam_cvVal2 :
    filter_1p_lo_in(&(cvSlew[2]), (v >> (PARAM_DAC_RADIX - 1))  & DAC_VALUE_MASK);
    //cv_update(2, v >> (PARAM_CV_RADIX - 1));
    break;
  case eParam_cvVal3 :
    filter_1p_lo_in(&(cvSlew[3]), (v >> (PARAM_DAC_RADIX - 1))  & DAC_VALUE_MASK);
    //cv_update(3, v >> (PARAM_CV_RADIX - 1));
    break;

  case eParam_cvSlew0 :
    filter_1p_lo_set_slew(&(cvSlew[0]), v);
    break;
  case eParam_cvSlew1 :
    filter_1p_lo_set_slew(&(cvSlew[1]), v);
    break;
  case eParam_cvSlew2 :
    filter_1p_lo_set_slew(&(cvSlew[2]), v);
    break;
  case eParam_cvSlew3 :
    filter_1p_lo_set_slew(&(cvSlew[3]), v);
    break;

   // i/o mix:
  case eParam_adc0_dac0 :
    mix_adc_dac[0][0] = v;
    //    ioAmp0 = v;
    break;
  case eParam_adc0_dac1 :
    mix_adc_dac[0][1] = v;
    break;
  case eParam_adc0_dac2 :
    mix_adc_dac[0][2] = v;
    break;
  case eParam_adc0_dac3 :
    mix_adc_dac[0][3] = v;
    break;
  case eParam_adc1_dac0 :
    mix_adc_dac[1][0] = v;
    break;
  case eParam_adc1_dac1 :
    mix_adc_dac[1][1] = v;
    //    ioAmp1 = v;
    break;
  case eParam_adc1_dac2 :
    mix_adc_dac[1][2] = v;
    break;
  case eParam_adc1_dac3 :
    mix_adc_dac[1][3] = v;
    break;
  case eParam_adc2_dac0 :
    mix_adc_dac[2][0] = v;
    break;
  case eParam_adc2_dac1 :
    mix_adc_dac[2][1] = v;
    break;
  case eParam_adc2_dac2 :
    mix_adc_dac[2][2] = v;
    //    ioAmp2 = v;
    break;
  case eParam_adc2_dac3 :
    mix_adc_dac[2][3] = v;
    break;
  case eParam_adc3_dac0 :
    mix_adc_dac[3][0] = v;
    break;
  case eParam_adc3_dac1 :
    mix_adc_dac[3][1] = v;
    break;
  case eParam_adc3_dac2 :
    mix_adc_dac[3][2] = v;
    break;
  case eParam_adc3_dac3 :
    mix_adc_dac[3][3] = v;
    //    ioAmp3 = v;
    break;
    // osc mix:
  case eParam_osc0_dac0 :
    mix_osc_dac[0][0] = v;
    break;
  case eParam_osc0_dac1 :
    mix_osc_dac[0][1] = v;
    break;
  case eParam_osc0_dac2 :
    mix_osc_dac[0][2] = v;
    break;
  case eParam_osc0_dac3 :
    mix_osc_dac[0][3] = v;
    break;
  case eParam_osc1_dac0 :
    mix_osc_dac[1][0] = v;
    break;
  case eParam_osc1_dac1 :
    mix_osc_dac[1][1] = v;
    break;
  case eParam_osc1_dac2 :
    mix_osc_dac[1][2] = v;
    break;
  case eParam_osc1_dac3 :
    mix_osc_dac[1][3] = v;
    break;


  default:
    break;
  }

}
