// this is its own file for pure convenience....

// check crossfade status of target
static u8 check_fade_rd(u8 id) {
  u8 newTarget, oldTarget = fadeTargetRd[id];
  //  u8 ret;

  if(lpFadeRd[id].sync) {
    // not fading right now, so pick different target and start crossfade
    newTarget =  oldTarget ^ 1;
    // copy all tap parameters to target
    buffer_tapN_copy( &(lines[id].tapRd[oldTarget]) ,  &(lines[id].tapRd[newTarget]) );
    // start the fade
    filter_ramp_tog_in(&(lpFadeRd[id]), newTarget);
    fadeTargetRd[id] = newTarget;
    return 1;
  } else {
    // fade is in progress.
    return !fadeIgnore;
  }
}

/*
static void check_fade_wr(u8 id) {
  if(lpFadeWr[id].sync) {
    // not fading right now, so pick different target and start crossfade
    fadeTargetWr[id] ^= 1;
    filter_1p_lo_in(&(lpFadeWr[id]), (fract32)((u32)(fadeTargetWr[id]) << 31) - 1);
  }
} 
*/ 

void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {
    // delay line params
  case eParam_delay0 :
    if( check_fade_rd(0) ) {
      delayFadeN_set_delay_sec(&(lines[0]), v,  fadeTargetRd[0]);
    }
    break;
  case eParam_delay1 :
    if(check_fade_rd(1)) {
      delayFadeN_set_delay_sec(&(lines[1]), v,  fadeTargetRd[1]);
    }
    break;
  case eParam_loop0 :
    delayFadeN_set_loop_sec(&(lines[0]), v, 0);
    delayFadeN_set_loop_sec(&(lines[0]), v, 1);
    break;
  case eParam_loop1 :
    delayFadeN_set_loop_sec(&(lines[1]), v , 0);
    delayFadeN_set_loop_sec(&(lines[1]), v , 1);
    break;
  case eParam_pos_write0 :
    // check_fade_wr(0);
    delayFadeN_set_pos_write_sec(&(lines[0]), v,  fadeTargetWr[0]);
    break;
  case eParam_pos_write1 :
    // check_fade_wr(1);
    delayFadeN_set_pos_write_sec(&(lines[1]), v,  fadeTargetWr[1] );
    break;
  case eParam_pos_read0 :
    if (check_fade_rd(0) ) {
      delayFadeN_set_pos_read_sec(&(lines[0]), v,  fadeTargetRd[0]);
    }
    break;
  case eParam_pos_read1 :
    if( check_fade_rd(1) ) {
      delayFadeN_set_pos_read_sec(&(lines[1]), v,  fadeTargetRd[1]);
    }
    break;
  case eParam_run_write0 :
    delayFadeN_set_run_write(&(lines[0]), v);
    break;
  case eParam_run_write1 :
    delayFadeN_set_run_write(&(lines[1]), v);
    break;
  case eParam_run_read0 :
    delayFadeN_set_run_read(&(lines[0]), v);
    break;
  case eParam_run_read1 :
    delayFadeN_set_run_read(&(lines[1]), v);
    break;
  case eParam_rMul0 :
    //    if( 
    check_fade_rd(0);
    delayFadeN_set_mul(&(lines[0]), v >> 16,  fadeTargetRd[0]);
    //    }
    break;
  case eParam_rDiv0 :
    //    if (
    check_fade_rd(0) ;
    //	) {
    delayFadeN_set_div(&(lines[0]), v >> 16,  fadeTargetRd[0]);
    //    }
    break;
  case eParam_rMul1 :
    //    if (
    check_fade_rd(1) ;
    //) {
    delayFadeN_set_mul(&(lines[1]), v >> 16 ,  fadeTargetRd[1]);
    //    }
    break;
  case eParam_rDiv1 :
    //    if (
    check_fade_rd(1) ;
	  //) {
    delayFadeN_set_div(&(lines[1]), v >> 16 ,  fadeTargetRd[1]);
	//    }
    break;
  case eParam_write0 :
    /// FIXME: need write level...
    delayFadeN_set_write(&(lines[0]), v > 0);
    break;
  case eParam_write1 :
    /// FIXME: need write level...gs
    delayFadeN_set_write(&(lines[1]), v > 0);
    break;
  case eParam_pre0 :
    if(v == FR32_MAX) {
      // negative == full
      delayFadeN_set_pre(&(lines[0]), -1);
    } else {
      delayFadeN_set_pre(&(lines[0]), v);
    }
    break;
  case eParam_pre1 :
    if(v == FR32_MAX) {
      // negative == full
      delayFadeN_set_pre(&(lines[1]), -1);
    } else {
      delayFadeN_set_pre(&(lines[1]), v);
    }
    break;
    // filter params
  case eParam_freq0 :
    //    filter_svf_set_coeff(&(svf[0]), v );
    filter_1p_lo_in(&(svfCutSlew[0]), v);
    break;
  case eParam_freq1 :
    //    filter_svf_set_coeff(&(svf[1]), v );
    filter_1p_lo_in(&(svfCutSlew[1]), v);
    break;
  case eParam_rq0 :
    //    filter_svf_set_rq(&(svf[0]), v);
    // incoming param value is 16.16
    // target is 2.30xs
    //    filter_svf_set_rq(&(svf[0]), v << 14);
    filter_1p_lo_in(&(svfRqSlew[0]), v << 14);
    break;
  case eParam_rq1 :
    //    filter_svf_set_rq(&(svf[1]), v);
    //    filter_svf_set_rq(&(svf[1]), v << 14);
    filter_1p_lo_in(&(svfRqSlew[1]), v << 14);
    break;
  case eParam_low0 :
    filter_svf_set_low(&(svf[0]), v);
    break;
  case eParam_low1 :
    filter_svf_set_low(&(svf[1]), v);
    break;
  case eParam_high0 :
    filter_svf_set_high(&(svf[0]), v);
    break;
  case eParam_high1 :
    filter_svf_set_high(&(svf[1]), v);
    break;
  case eParam_band0 :
    filter_svf_set_band(&(svf[0]), v);
    break;
  case eParam_band1 :
    filter_svf_set_band(&(svf[1]), v);
    break;
  case eParam_notch0 :
    filter_svf_set_notch(&(svf[0]), v);
    break;
  case eParam_notch1 :
    filter_svf_set_notch(&(svf[1]), v);
    break;

    // filter balance
  case eParam_fwet0 :
    mix_fwet[0] = v;
    break;

  case eParam_fwet1 :
    mix_fwet[1] = v;
    break;

  case eParam_fdry0 :
    mix_fdry[0] = v;
    break;

  case eParam_fdry1 :
    mix_fdry[1] = v;
    break;

    /// mix points
  case eParam_adc0_del0 :
    mix_adc_del[0][0] = v;
    break;
  case eParam_adc0_del1 :
    mix_adc_del[0][1] = v;
    break;
  case eParam_adc1_del0 :
    mix_adc_del[1][0] = v;
    break;
  case eParam_adc1_del1 :
    mix_adc_del[1][1] = v;
    break;
  case eParam_adc2_del0 :
    mix_adc_del[2][0] = v;
    break;
  case eParam_adc2_del1 :
    mix_adc_del[2][1] = v;
    break;
  case eParam_adc3_del0 :
    mix_adc_del[3][0] = v;
    break;
  case eParam_adc3_del1 :
    mix_adc_del[3][1] = v;
    break;
  case eParam_del0_del0 :
    mix_del_del[0][0] = v;
    break;
  case eParam_del0_del1 :
    mix_del_del[0][1] = v;
    break;
  case eParam_del1_del0 :
    mix_del_del[1][0] = v;
    break;
  case eParam_del1_del1 :
    mix_del_del[1][1] = v;
    break;
    // dry :
  case eParam_adc0_dac0 :
    mix_adc_dac[0][0] = v;
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
    break;
    // wet :
  case eParam_del0_dac0 :
    mix_del_dac[0][0] = v;
    break;
  case eParam_del0_dac1 :
    mix_del_dac[0][1] = v;
    break;
  case eParam_del0_dac2 :
    mix_del_dac[0][2] = v;
    break;
  case eParam_del0_dac3 :
    mix_del_dac[0][3] = v;
    break;
  case eParam_del1_dac0 :
    mix_del_dac[1][0] = v;
    break;
  case eParam_del1_dac1 :
    mix_del_dac[1][1] = v;
    break;
  case eParam_del1_dac2 :
    mix_del_dac[1][2] = v;
    break;
  case eParam_del1_dac3 :
    mix_del_dac[1][3] = v;
    break;

    // param integrators
  case eParamCut0Slew :
    filter_1p_lo_set_slew(&(svfCutSlew[0]), v);
    break;
  case eParamCut1Slew :
    filter_1p_lo_set_slew(&(svfCutSlew[1]), v);
    break;

  case eParamRq0Slew :
    filter_1p_lo_set_slew(&(svfRqSlew[0]), v);
    break;
  case eParamRq1Slew :
    filter_1p_lo_set_slew(&(svfRqSlew[1]), v);
    break;

    // -- cv output
    // cv values
  case eParam_cvVal0 :
    filter_1p_lo_in(&(cvSlew[0]), v) ;
    break;
  case eParam_cvVal1 :
    filter_1p_lo_in(&(cvSlew[1]), v) ;
    break;
  case eParam_cvVal2 :
    filter_1p_lo_in(&(cvSlew[2]), v) ;
    break;
  case eParam_cvVal3 :
    filter_1p_lo_in(&(cvSlew[3]), v) ;
    break;

    // cv slew
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

    // fade times
    // FIXME: value is raw ramp increment per sample...
  case eParamFade0 :
    filter_ramp_tog_set_inc(&(lpFadeRd[0]), v );
    //    filter_ramp_tog_set_inc(&(lpFadeWr[0]), v );
    break;
  case eParamFade1 :
    filter_ramp_tog_set_inc(&(lpFadeRd[1]), v );
    //    filter_ramp_tog_set_inc(&(lpFadeWr[1]), v);
    break;

  default:
    break;
  }
}

// EOF
