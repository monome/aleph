// this is its own file for pure convenience.

void module_set_param(u32 idx, pval v) {
  switch(idx) {
    // delay line params
  case eParam_delay0 :
    delay_set_delay_samp(&(lines[0]), v.u);
    break;
  case eParam_delay1 :
    delay_set_delay_samp(&(lines[1]), v.u);
    break;
  case eParam_loop0 :
    delay_set_loop_samp(&(lines[0]), v.u);
    break;
  case eParam_loop1 :
    delay_set_loop_samp(&(lines[1]), v.u);
    break;
  case eParam_pos_write0 :
    delay_set_pos_write_samp(&(lines[0]), v.u);
    break;
  case eParam_pos_write1 :
    delay_set_pos_write_samp(&(lines[1]), v.u);
    break;
  case eParam_pos_read0 :
    delay_set_pos_read_samp(&(lines[0]), v.u);
    break;
  case eParam_pos_read1 :
    delay_set_pos_read_samp(&(lines[1]), v.u);
    break;
  case eParam_run_write0 :
    delay_set_run_write(&(lines[0]), v.fr);
    break;
  case eParam_run_write1 :
    delay_set_run_write(&(lines[1]), v.fr);
    break;
  case eParam_run_read0 :
    delay_set_run_read(&(lines[0]), v.fr);
    break;
  case eParam_run_read1 :
    delay_set_run_read(&(lines[1]), v.fr);
    break;
  case eParam_rate0 :
    /// FIXME
    //    delay_set_rate(&(lines[0]), v.fix);
    break;
  case eParam_rate1 :
    /// FIXME
    //    delay_set_rate(&(lines[1]), v.fix);
    break;
  case eParam_write0 :
    delay_set_write(&(lines[0]), v.s > 0);
    break;
  case eParam_write1 :
    delay_set_write(&(lines[1]), v.fr > 0);
    break;
  case eParam_pre0 :
    if(v.fr == FR32_MAX) {
      // negative == full
      delay_set_pre(&(lines[0]), -1);
    } else {
      delay_set_pre(&(lines[0]), v.fr);
    }
    break;
  case eParam_pre1 :
    if(v.fr == FR32_MAX) {
      // negative == full
      delay_set_pre(&(lines[1]), -1);
    } else {
      delay_set_pre(&(lines[1]), v.fr);
    }
    break;
    // filter params
  case eParam_coeff0 :
    filter_svf_set_coeff( &(svf[0]), v.fr );
    break;
  case eParam_coeff1 :
    filter_svf_set_coeff(&(svf[1]), v.fr );
    break;
  case eParam_rq0 :
    filter_svf_set_rq(&(svf[0]), v.fr);
    break;
  case eParam_rq1 :
    filter_svf_set_rq(&(svf[1]), v.fr);
    break;
  case eParam_low0 :
    filter_svf_set_low(&(svf[0]), v.fr);
    break;
  case eParam_low1 :
    filter_svf_set_low(&(svf[1]), v.fr);
    break;
  case eParam_high0 :
    filter_svf_set_high(&(svf[0]), v.fr);
    break;
  case eParam_high1 :
    filter_svf_set_high(&(svf[1]), v.fr);
    break;
  case eParam_band0 :
    filter_svf_set_band(&(svf[0]), v.fr);
    break;
  case eParam_band1 :
    filter_svf_set_band(&(svf[1]), v.fr);
    break;
  case eParam_notch0 :
    filter_svf_set_notch(&(svf[0]), v.fr);
    break;
  case eParam_notch1 :
    filter_svf_set_notch(&(svf[1]), v.fr);
    break;
    // filter balance
  case eParam_mix0 :
    break;
    mix_svf[0] = v.fr;
    break;
  case eParam_mix1 :
    mix_svf[1] = v.fr;
    break;
    /// mix points
  case eParam_adc0_del0 :
    mix_adc_del[0][0] = v.fr;
    break;
  case eParam_adc0_del1 :
    mix_adc_del[0][1] = v.fr;
    break;
  case eParam_adc1_del0 :
    mix_adc_del[1][0] = v.fr;
    break;
  case eParam_adc1_del1 :
    mix_adc_del[1][1] = v.fr;
    break;
  case eParam_adc2_del0 :
    mix_adc_del[2][0] = v.fr;
    break;
  case eParam_adc2_del1 :
    mix_adc_del[2][1] = v.fr;
    break;
  case eParam_adc3_del0 :
    mix_adc_del[3][0] = v.fr;
    break;
  case eParam_adc3_del1 :
    mix_adc_del[3][1] = v.fr;
    break;
  case eParam_del0_del0 :
    mix_del_del[0][0] = v.fr;
    break;
  case eParam_del0_del1 :
    mix_del_del[0][1] = v.fr;
    break;
  case eParam_del1_del0 :
    mix_del_del[1][0] = v.fr;
    break;
  case eParam_del1_del1 :
    mix_del_del[1][1] = v.fr;
    break;
    // dry :
  case eParam_adc0_dac0 :
    mix_adc_dac[0][0] = v.fr;
    break;
  case eParam_adc0_dac1 :
    mix_adc_dac[0][1] = v.fr;
    break;
  case eParam_adc0_dac2 :
    mix_adc_dac[0][2] = v.fr;
    break;
  case eParam_adc0_dac3 :
    mix_adc_dac[0][3] = v.fr;
    break;
  case eParam_adc1_dac0 :
    mix_adc_dac[1][0] = v.fr;
    break;
  case eParam_adc1_dac1 :
    mix_adc_dac[1][1] = v.fr;
    break;
  case eParam_adc1_dac2 :
    mix_adc_dac[1][2] = v.fr;
    break;
  case eParam_adc1_dac3 :
    mix_adc_dac[1][3] = v.fr;
    break;
  case eParam_adc2_dac0 :
    mix_adc_dac[2][0] = v.fr;
    break;
  case eParam_adc2_dac1 :
    mix_adc_dac[2][1] = v.fr;
    break;
  case eParam_adc2_dac2 :
    mix_adc_dac[2][2] = v.fr;
    break;
  case eParam_adc2_dac3 :
    mix_adc_dac[2][3] = v.fr;
    break;
  case eParam_adc3_dac0 :
    mix_adc_dac[3][0] = v.fr;
    break;
  case eParam_adc3_dac1 :
    mix_adc_dac[3][1] = v.fr;
    break;
  case eParam_adc3_dac2 :
    mix_adc_dac[3][2] = v.fr;
    break;
  case eParam_adc3_dac3 :
    mix_adc_dac[3][3] = v.fr;
    break;
    // wet :
  case eParam_del0_dac0 :
    mix_del_dac[0][0] = v.fr;
    break;
  case eParam_del0_dac1 :
    mix_del_dac[0][1] = v.fr;
    break;
  case eParam_del0_dac2 :
    mix_del_dac[0][2] = v.fr;
    break;
  case eParam_del0_dac3 :
    mix_del_dac[0][3] = v.fr;
    break;
  case eParam_del1_dac0 :
    mix_del_dac[1][0] = v.fr;
    break;
  case eParam_del1_dac1 :
    mix_del_dac[1][1] = v.fr;
    break;
  case eParam_del1_dac2 :
    mix_del_dac[1][2] = v.fr;
    break;
  case eParam_del1_dac3 :
    mix_del_dac[1][3] = v.fr;
    break;

  default:
    break;
  }
}

// EOF


