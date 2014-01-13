// this is its own file for pure convenience.

void module_set_param(u32 idx, ParamValue v) {
  switch(idx) {
    // delay line params
  case eParam_delay0 :
    delay_set_delay_sec(&(lines[0]), v);
    break;
  case eParam_delay1 :
    delay_set_delay_sec(&(lines[1]), v);
    break;
  case eParam_loop0 :
    delay_set_loop_sec(&(lines[0]), v);
    break;
  case eParam_loop1 :
    delay_set_loop_sec(&(lines[1]), v);
    break;
  case eParam_pos_write0 :
    delay_set_pos_write_sec(&(lines[0]), v);
    break;
  case eParam_pos_write1 :
    delay_set_pos_write_sec(&(lines[1]), v);
    break;
  case eParam_pos_read0 :
    delay_set_pos_read_sec(&(lines[0]), v);
    break;
  case eParam_pos_read1 :
    delay_set_pos_read_sec(&(lines[1]), v);
    break;
  case eParam_run_write0 :
    delay_set_run_write(&(lines[0]), v);
    break;
  case eParam_run_write1 :
    delay_set_run_write(&(lines[1]), v);
    break;
  case eParam_run_read0 :
    delay_set_run_read(&(lines[0]), v);
    break;
  case eParam_run_read1 :
    delay_set_run_read(&(lines[1]), v);
    break;
  case eParam_rMul0 :
    /// FIXME
    //    delay_set_rate(&(lines[0]), v.fix);
    break;
  case eParam_rDiv0 :
    /// FIXME
    //    delay_set_rate(&(lines[0]), v.fix);
    break;

  case eParam_rMul1 :
    /// FIXME
    //    delay_set_rate(&(lines[1]), v.fix);
    break;
  case eParam_rDiv1 :
    /// FIXME
    //    delay_set_rate(&(lines[1]), v.fix);
    break;
  case eParam_write0 :
    delay_set_write(&(lines[0]), v > 0);
    break;
  case eParam_write1 :
    delay_set_write(&(lines[1]), v > 0);
    break;
  case eParam_pre0 :
    if(v == FR32_MAX) {
      // negative == full
      delay_set_pre(&(lines[0]), -1);
    } else {
      delay_set_pre(&(lines[0]), v);
    }
    break;
  case eParam_pre1 :
    if(v == FR32_MAX) {
      // negative == full
      delay_set_pre(&(lines[1]), -1);
    } else {
      delay_set_pre(&(lines[1]), v);
    }
    break;
    // filter params
  case eParam_freq0 :
    filter_svf_set_coeff(&(svf[0]), v );
    break;
  case eParam_freq1 :
    filter_svf_set_coeff(&(svf[1]), v );
    break;
  case eParam_rq0 :
    // incoming param value is 16.16
    // target is 2.30
    //    filter_svf_set_rq(&(svf[0]), v);
    filter_svf_set_rq(&(svf[0]), v << 14);
    break;
  case eParam_rq1 :
    //    filter_svf_set_rq(&(svf[1]), v);
    filter_svf_set_rq(&(svf[1]), v << 14);
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

  default:
    break;
  }
}

// EOF


