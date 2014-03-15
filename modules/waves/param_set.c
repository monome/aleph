// set param values.
// this is a separate file for convenience only.

// convert fractional 16.16 value to fr16
static inline fract16 param_to_fr16( ParamValue v) {
  return (fract16)((v & 0xffff) >> 1);
}

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
    osc_set_shape( &(voice[1].osc),  param_to_fr16(v) );
    break;
  case eParamWave0:
    osc_set_shape( &(voice[0].osc),  param_to_fr16(v) );
    break;

    // FIXME: this is assuming simple fixed x-modulation with 2 voices.
    /// should have a matrix of modulation mix points.
  case eParamPm10:
    osc_set_pm ( &(voice[0].osc), param_to_fr16(v) );
    break;
  case eParamPm01:
    osc_set_pm ( &(voice[1].osc), param_to_fr16(v) );
    break;

  case eParamWm10:
    osc_set_wm ( &(voice[0].osc), param_to_fr16(v) );
    break;
  case eParamWm01:
    osc_set_wm ( &(voice[1].osc), param_to_fr16(v) );
    break;

  case eParamAmp1:
    //    filter_1p_lo_in(&(voice[1].ampSlew), v);
    voice[1].ampSlew.x = v;
    break;
  case eParamAmp0:
    //    filter_1p_lo_in(&(voice[0].ampSlew), v);
    voice[0].ampSlew.x = v;
    break;

  /* case eParamBl1 : */
  /*   //    osc_set_bl( &(voice[1].osc), param_to_fr16(v) ); */
  /*   break; */
  /* case eParamBl0 : */
  /*   //    osc_set_bl( &(voice[0].osc), param_to_fr16(v) ); */
  /*   break; */

    //// filter params:
  case eParam_cut1 :
    //    filter_1p_lo_in( &(voice[1].cutSlew), v );
    voice[1].cutSlew.x = v;
    break;
  case eParam_cut0 :
    //    filter_1p_lo_in( &(voice[0].cutSlew), v );
    voice[0].cutSlew.x = v;
    break;
  case eParam_rq1 :
    //    filter_1p_lo_in( &(voice[1].rqSlew), v << 14 ); 
    voice[1].rqSlew.x = v << 14;
    break;
  case eParam_rq0 :
    //    filter_1p_lo_in( &(voice[0].rqSlew), v << 14 );
    voice[0].rqSlew.x = v << 14;

    break;
  case eParam_low1 :
    filter_svf_set_low(&(voice[1].svf), trunc_fr1x32(v));
    break;
  case eParam_low0 :
    filter_svf_set_low(&(voice[0].svf), trunc_fr1x32(v));
    break;
  case eParam_high1 :
    filter_svf_set_high(&(voice[1].svf), trunc_fr1x32(v));
    break;
  case eParam_high0 :
    filter_svf_set_high(&(voice[0].svf), trunc_fr1x32(v));
    break;
  case eParam_band1 :
    filter_svf_set_band(&(voice[1].svf), trunc_fr1x32(v));
    break;
  case eParam_band0 :
    filter_svf_set_band(&(voice[0].svf), trunc_fr1x32(v));
    break;
  case eParam_notch1 :
    filter_svf_set_notch(&(voice[1].svf), trunc_fr1x32(v));
    break;
  case eParam_notch0 :
    filter_svf_set_notch(&(voice[0].svf), trunc_fr1x32(v));
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

    //----- slew parameters
    
    // oscillator param slew
  case eParamHz1Slew:
    voice[1].osc.incSlew.c = v;
    break;
  case eParamHz0Slew:
    voice[0].osc.incSlew.c = v;
    break;

  case eParamPm01Slew:
    voice[1].osc.pmSlew.c = trunc_fr1x32(v);
    break;
  case eParamPm10Slew:
    voice[0].osc.pmSlew.c = trunc_fr1x32(v);
    break;

  case eParamWm01Slew:
    voice[1].osc.wmSlew.c = trunc_fr1x32(v);
    break;
  case eParamWm10Slew:
    voice[0].osc.wmSlew.c = trunc_fr1x32(v);
    break;

  case eParamWave1Slew:
    break;
  case eParamWave0Slew:
    break;
  case eParamAmp1Slew:
    voice[1].ampSlew.c = v;
    break;
  case eParamAmp0Slew:
    voice[0].ampSlew.c = v;
    break;

    // other param slew
  case eParamCut0Slew :
    //    filter_1p_lo_set_slew(&(voice[0].cutSlew), v);
    voice[0].cutSlew.c = v;
    break;
  case eParamCut1Slew :
    voice[1].cutSlew.c = v;
    break;

  case eParamRq0Slew :
    voice[0].rqSlew.c = v;
    break;
  case eParamRq1Slew :
    voice[1].rqSlew.c = v;
    break;

    // cv values
  case eParam_cvVal0 :
    //    filter_1p_lo_in(&(cvSlew[0]), (v >> ( PARAM_DAC_RADIX - 1))  & DAC_VALUE_MASK);
    cvSlew[0].x = (v >> (PARAM_DAC_SHIFT))  & DAC_VALUE_MASK;
    break;
  case eParam_cvVal1 :
    cvSlew[1].x = (v >> (PARAM_DAC_SHIFT)) & DAC_VALUE_MASK;
    break;
  case eParam_cvVal2 :
    cvSlew[2].x = (v >> (PARAM_DAC_SHIFT))  & DAC_VALUE_MASK;
    break;
  case eParam_cvVal3 :
    cvSlew[3].x = (v >> (PARAM_DAC_SHIFT))  & DAC_VALUE_MASK;
    break;

  case eParam_cvSlew0 :
    cvSlew[0].c = v;
    break;
  case eParam_cvSlew1 :
    cvSlew[1].c = v;
    break;
  case eParam_cvSlew2 :
    cvSlew[2].c = v;
    break;
  case eParam_cvSlew3 :
    cvSlew[3].c = v;
    break;

   // i/o mix:
  case eParam_adc0_dac0 :
    mix_adc_dac[0][0] = trunc_fr1x32(v);
    break;
  case eParam_adc0_dac1 :
    mix_adc_dac[0][1] = trunc_fr1x32(v);
    break;
  case eParam_adc0_dac2 :
    mix_adc_dac[0][2] = trunc_fr1x32(v);
    break;
  case eParam_adc0_dac3 :
    mix_adc_dac[0][3] = trunc_fr1x32(v);
    break;
  case eParam_adc1_dac0 :
    mix_adc_dac[1][0] = trunc_fr1x32(v);
    break;
  case eParam_adc1_dac1 :
    mix_adc_dac[1][1] = trunc_fr1x32(v);
    break;
  case eParam_adc1_dac2 :
    mix_adc_dac[1][2] = trunc_fr1x32(v);
    break;
  case eParam_adc1_dac3 :
    mix_adc_dac[1][3] = trunc_fr1x32(v);
    break;
  case eParam_adc2_dac0 :
    mix_adc_dac[2][0] = trunc_fr1x32(v);
    break;
  case eParam_adc2_dac1 :
    mix_adc_dac[2][1] = trunc_fr1x32(v);
    break;
  case eParam_adc2_dac2 :
    mix_adc_dac[2][2] = trunc_fr1x32(v);
    break;
  case eParam_adc2_dac3 :
    mix_adc_dac[2][3] = trunc_fr1x32(v);
    break;
  case eParam_adc3_dac0 :
    mix_adc_dac[3][0] = trunc_fr1x32(v);
    break;
  case eParam_adc3_dac1 :
    mix_adc_dac[3][1] = trunc_fr1x32(v);
    break;
  case eParam_adc3_dac2 :
    mix_adc_dac[3][2] = trunc_fr1x32(v);
    break;
  case eParam_adc3_dac3 :
    mix_adc_dac[3][3] = trunc_fr1x32(v);
    break;

    // osc mix:
  case eParam_osc0_dac0 :
    mix_osc_dac[0][0] = trunc_fr1x32(v);
    break;
  case eParam_osc0_dac1 :
    mix_osc_dac[0][1] = trunc_fr1x32(v);
    break;
  case eParam_osc0_dac2 :
    mix_osc_dac[0][2] = trunc_fr1x32(v);
    break;
  case eParam_osc0_dac3 :
    mix_osc_dac[0][3] = trunc_fr1x32(v);
    break;
  case eParam_osc1_dac0 :
    mix_osc_dac[1][0] = trunc_fr1x32(v);
    break;
  case eParam_osc1_dac1 :
    mix_osc_dac[1][1] = trunc_fr1x32(v);
    break;
  case eParam_osc1_dac2 :
    mix_osc_dac[1][2] = trunc_fr1x32(v);
    break;
  case eParam_osc1_dac3 :
    mix_osc_dac[1][3] = trunc_fr1x32(v);
    break;

  default:
    break;
  }

}
