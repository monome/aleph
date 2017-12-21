// this is its own file for pure convenience....

static u8 start_fade_rd(u8 id) {
  u8 newTarget, oldTarget;
  //  u8 ret;

  if(lpFadeRd[id].sync) {
    /* printf("heading to %d,%d\n", fadeTargetRd[id], id); */
    // not fading right now, so pick different target and start crossfade
    oldTarget = fadeTargetRd[id];
    newTarget = !oldTarget;
    // copy all tap parameters to target
    buffer_tapN_copy( &(lines[id].tapRd[oldTarget]),
		      &(lines[id].tapRd[newTarget]) );
    fadeTargetRd[id] = newTarget;
    // start the fade
    filter_ramp_start(&(lpFadeRd[id]));
    return 1;
  } else {
    // fade is in progress.
    return 0;
  }
}

void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {
    // delay line params
  case eParam_delay0 :
    if( start_fade_rd(0) ) {
      delayFadeN_set_delay_ms(&(lines[0]),
			      calc_ms(trunc_fr1x32(v)),
			      fadeTargetRd[0]);
    }
    break;
  case eParam_delay1 :
    if(start_fade_rd(1)) {
      delayFadeN_set_delay_ms(&(lines[1]),
			      calc_ms(trunc_fr1x32(v)),
			      fadeTargetRd[1]);
    }
    break;
  case eParam_loop0 :
    delayFadeN_set_loop_ms(&(lines[0]),
			   calc_ms(trunc_fr1x32(v)));
    break;
  case eParam_loop1 :
    delayFadeN_set_loop_ms(&(lines[1]),
			   calc_ms(trunc_fr1x32(v)));
    break;
  case eParam_pos_write0 :
    delayFadeN_set_pos_write_ms(&(lines[0]),
				calc_ms(trunc_fr1x32(v)));
    break;
  case eParam_pos_write1 :
    delayFadeN_set_pos_write_ms(&(lines[1]),
				calc_ms(trunc_fr1x32(v)));
    break;
  case eParam_pos_read0 :
    if (start_fade_rd(0) ) {
      delayFadeN_set_pos_read_ms(&(lines[0]),
				 calc_ms(trunc_fr1x32(v)),
				 fadeTargetRd[0]);
    }
    break;
  case eParam_pos_read1 :
    if( start_fade_rd(1) ) {
      delayFadeN_set_pos_read_ms(&(lines[1]),
				 calc_ms(trunc_fr1x32(v)),
				 fadeTargetRd[1]);
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
    delayFadeN_set_mul(&(lines[0]), v >> 16);
    break;
  case eParam_rDiv0 :
    delayFadeN_set_div(&(lines[0]), v >> 16);
    break;
  case eParam_rMul1 :
    delayFadeN_set_mul(&(lines[1]), v >> 16);
    break;
  case eParam_rDiv1 :
    delayFadeN_set_div(&(lines[1]), v >> 16);
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
    svfCutSlew[0].x = v;
    break;
  case eParam_freq1 :
    //    filter_svf_set_coeff(&(svf[1]), v );
    svfCutSlew[1].x = v;
    break;
  case eParam_rq0 :
    //    filter_svf_set_rq(&(svf[0]), v);
    // incoming param value is 16.16
    // target is 2.30xs
    //    filter_svf_set_rq(&(svf[0]), v << 14);
    svfRqSlew[0].x = v << 14;
    break;
  case eParam_rq1 :
    //    filter_svf_set_rq(&(svf[1]), v);
    //    filter_svf_set_rq(&(svf[1]), v << 14);
    svfRqSlew[1].x = v << 14;
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
    /* mix_fwet[0] = v; */
    wetSlew[0].x = v;
    break;

  case eParam_fwet1 :
    /* mix_fwet[1] = v; */
    wetSlew[1].x = v;
    break;

  case eParam_fdry0 :
    /* mix_fdry[0] = v; */
    drySlew[0].x = v;
    break;

  case eParam_fdry1 :
    /* mix_fdry[1] = v; */
    drySlew[1].x = v;
    break;

    /// mix points
  case eParam_adc0_del0 :
    mix_adc_del[0][0] = trunc_fr1x32(v);
    break;
  case eParam_adc0_del1 :
    mix_adc_del[0][1] = trunc_fr1x32(v);
    break;
  case eParam_adc1_del0 :
    mix_adc_del[1][0] = trunc_fr1x32(v);
    break;
  case eParam_adc1_del1 :
    mix_adc_del[1][1] = trunc_fr1x32(v);
    break;
  case eParam_adc2_del0 :
    mix_adc_del[2][0] = trunc_fr1x32(v);
    break;
  case eParam_adc2_del1 :
    mix_adc_del[2][1] = trunc_fr1x32(v);
    break;
  case eParam_adc3_del0 :
    mix_adc_del[3][0] = trunc_fr1x32(v);
    break;
  case eParam_adc3_del1 :
    mix_adc_del[3][1] = trunc_fr1x32(v);
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
    // wet :
  case eParam_del0_dac0 :
    mix_del_dac[0][0] = trunc_fr1x32(v);
    break;
  case eParam_del0_dac1 :
    mix_del_dac[0][1] = trunc_fr1x32(v);
    break;
  case eParam_del0_dac2 :
    mix_del_dac[0][2] = trunc_fr1x32(v);
    break;
  case eParam_del0_dac3 :
    mix_del_dac[0][3] = trunc_fr1x32(v);
    break;
  case eParam_del1_dac0 :
    mix_del_dac[1][0] = trunc_fr1x32(v);
    break;
  case eParam_del1_dac1 :
    mix_del_dac[1][1] = trunc_fr1x32(v);
    break;
  case eParam_del1_dac2 :
    mix_del_dac[1][2] = trunc_fr1x32(v);
    break;
  case eParam_del1_dac3 :
    mix_del_dac[1][3] = trunc_fr1x32(v);
    break;

    // param integrators
  case eParamCut0Slew :
    svfCutSlew[0].c =  v;
    break;
  case eParamCut1Slew :
    svfCutSlew[1].c =  v;
    break;

  case eParamRq0Slew :
    svfRqSlew[0].c =  v;
    break;
  case eParamRq1Slew :
    svfRqSlew[1].c =  v;
    break;

    // -- cv output
    // cv values
  case eParam_cvVal0 :
    cvSlew[0].x = v;
    break;
  case eParam_cvVal1 :
    cvSlew[1].x = v;
    break;
  case eParam_cvVal2 :
    cvSlew[2].x = v;
    break;
  case eParam_cvVal3 :
    cvSlew[3].x = v;
    break;

    // cv slew
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

  case eParamDry0Slew :
    drySlew[0].c = v;
    break;
  case eParamWet0Slew :
    wetSlew[0].c = v;
    break;
  case eParamDry1Slew :
    drySlew[1].c = v;
    break;
  case eParamWet1Slew :
    wetSlew[1].c = v;
    break;

    // fade times
    // FIXME: value is raw ramp increment per sample...
  case eParamFade0 :
    if (v > PARAM_FADE_MIN) { // max fade 5s
      filter_ramp_set_inc(&(lpFadeRd[0]), v );
    }
    //    filter_ramp_tog_set_inc(&(lpFadeWr[0]), v );
    break;
  case eParamFade1 :
    if (v > PARAM_FADE_MIN) { // max fade 5s
      filter_ramp_set_inc(&(lpFadeRd[1]), v );
    }
    //    filter_ramp_tog_set_inc(&(lpFadeWr[1]), v);
    break;

  case eParamTimescale :
    globalTimescale = v >> 6;// Huh, shouldn't this be >> 4?
    //    filter_ramp_tog_set_inc(&(lpFadeWr[1]), v);
    break;

  default:
    break;
  }
}

// EOF
