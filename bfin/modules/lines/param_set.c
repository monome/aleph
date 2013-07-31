// this is its own file for pure convenience.

void module_set_param(u32 idx, pval v) {
  switch(idx) {
  case eParam_delay0 :
    delay_set_delay(&(lines[0]), v.fix);
    break;
  case eParam_delay1 :
    delay_set_delay(&(lines[1]), v.fix);
    break;
    /* case eParam_delay2 : */
    /*   delay_set_delay(&lines[2], v.fix); */
    /*   break; */
    /* case eParam_delay3 : */
    /*   delay_set_delay(&lines[3 ], v.fix); */
    /*   break; */
  case eParam_loop0 :
    delay_set_loop(&(lines[0]), v.fix);
    break;
  case eParam_loop1 :
    delay_set_loop(&(lines[1]), v.fix);
    break;
    /* case eParam_loop2 : */
    /*   delay_set_loop(&lines[2], v.fix); */
    /*   break; */
    /* case eParam_loop3 : */
    /*   delay_set_loop(&lines[3], v.fix); */
    /*   break; */
  case eParam_pos_write0 :
    //    delay_set_pos(&(lines[0]), v.fix);
    break;
  case eParam_pos_write1 :
    //    delay_set_pos(&(lines[1]), v.fix);
    break;
    /* case eParam_pos_write2 : */
    /*   //    delay_set_pos(&lines[2], v.fix); */
    /*   break; */
    /* case eParam_pos_write3 : */
    /*   //    delay_set_pos(&lines[3], v.fix); */
    /*   break; */
  case eParam_pos_read0 :
    //    delay_set_pos(&(lines[0]), v.fix);
    break;
  case eParam_pos_read1 :
    //    delay_set_pos(&(lines[1]), v.fix);
    break;
    /* case eParam_pos_read2 : */
    /*   //    delay_set_pos(&lines[2], v.fix); */
    /*   break; */
    /* case eParam_pos_read3 : */
    /*   //    delay_set_pos(&lines[3], v.fix); */
    /*   break; */
  case eParam_rate0 :
    //    delay_set_rate(&(lines[0]), v.fix);
    break;
  case eParam_rate1 :
    //    delay_set_rate(&(lines[1]), v.fix);
    break;
    /* case eParam_rate2 : */
    /*   //    delay_set_rate(&lines[2], v.fix); */
    /*   break; */
    /* case eParam_rate3 : */
    /*   //    delay_set_rate(&lines[3], v.fix); */
    /*   break; */
  case eParam_write0 :
    delay_set_write(&(lines[0]), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParam_write1 :
    delay_set_write(&(lines[1]), FIX16_FRACT_TRUNC(v.fix));
    break;
    /* case eParam_write2 : */
    /*   delay_set_write(&lines[2], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
    /* case eParam_write3 : */
    /*   delay_set_write(&lines[3] , FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
  case eParam_erase0 :
    delay_set_erase(&(lines[0]), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParam_erase1 :
    delay_set_erase(&(lines[1]), FIX16_FRACT_TRUNC(v.fix));
    break;
    /* case eParam_erase2 : */
    /*   delay_set_erase(&lines[2], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
    /* case eParam_erase3 : */
    /*   delay_set_erase(&lines[3], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
  case eParam_hz0 :
    filter_svf_set_hz(&(svf[0]), v.fix);
    break;
  case eParam_hz1 :
    filter_svf_set_hz(&(svf[1]), v.fix);
    break;
    /* case eParam_hz2 : */
    /*   filter_svf_set_hz(&svf[2], v.fix); */
    /*   break; */
    /* case eParam_hz3 : */
    /*   filter_svf_set_hz(&svf[3], v.fix); */
    /*   break; */
  case eParam_rq0 :
    filter_svf_set_rq(&(svf[0]), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParam_rq1 :
    filter_svf_set_rq(&(svf[1]), FIX16_FRACT_TRUNC(v.fix));
    break;
    /* case eParam_rq2 : */
    /*   filter_svf_set_rq(&svf[2], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
    /* case eParam_rq3 : */
    /*   filter_svf_set_rq(&svf[3], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
  case eParam_low0 :
    filter_svf_set_low(&(svf[0]), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParam_low1 :
    filter_svf_set_low(&(svf[1]), FIX16_FRACT_TRUNC(v.fix));
    break;
    /* case eParam_low2 : */
    /*   filter_svf_set_low(&svf[2], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
    /* case eParam_low3 : */
    /*   filter_svf_set_low(&svf[3], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
  case eParam_high0 :
    filter_svf_set_high(&(svf[0]), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParam_high1 :
    filter_svf_set_high(&(svf[1]), FIX16_FRACT_TRUNC(v.fix));
    break;
    /* case eParam_high2 : */
    /*   filter_svf_set_high(&svf[2], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
    /* case eParam_high3 : */
    /*   filter_svf_set_high(&svf[3], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
  case eParam_band0 :
    filter_svf_set_band(&(svf[0]), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParam_band1 :
    filter_svf_set_band(&(svf[1]), FIX16_FRACT_TRUNC(v.fix));
    break;
    /* case eParam_band2 : */
    /*   filter_svf_set_band(&svf[2], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
    /* case eParam_band3 : */
    /*   filter_svf_set_band(&svf[3], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
  case eParam_notch0 :
    filter_svf_set_notch(&(svf[0]), FIX16_FRACT_TRUNC(v.fix));
    break;
  case eParam_notch1 :
    filter_svf_set_notch(&(svf[1]), FIX16_FRACT_TRUNC(v.fix));
    break;
    /* case eParam_notch2 : */
    /*   filter_svf_set_notch(&svf[2], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */
    /* case eParam_notch3 : */
    /*   filter_svf_set_notch(&svf[3], FIX16_FRACT_TRUNC(v.fix)); */
    /*   break; */

  case eParam_adc0_del0 :
    mix_adc_del[0][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc0_del1 :
    mix_adc_del[0][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc1_del0 :
    mix_adc_del[1][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc1_del1 :
    mix_adc_del[1][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc2_del0 :
    mix_adc_del[2][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc2_del1 :
    mix_adc_del[2][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc3_del0 :
    mix_adc_del[3][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc3_del1 :
    mix_adc_del[3][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del0_del0 :
    mix_del_del[0][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del0_del1 :
    mix_del_del[0][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del1_del0 :
    mix_del_del[1][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del1_del1 :
    mix_del_del[1][1] = FIX16_FRACT_TRUNC(v.fix);
    break;

  case eParam_adc0_dac0 :
    mix_adc_dac[0][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc0_dac1 :
    mix_adc_dac[0][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc0_dac2 :
    mix_adc_dac[0][2] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc0_dac3 :
    mix_adc_dac[0][3] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc1_dac0 :
    mix_adc_dac[1][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc1_dac1 :
    mix_adc_dac[1][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc1_dac2 :
    mix_adc_dac[1][2] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc1_dac3 :
    mix_adc_dac[1][3] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc2_dac0 :
    mix_adc_dac[2][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc2_dac1 :
    mix_adc_dac[2][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc2_dac2 :
    mix_adc_dac[2][2] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc2_dac3 :
    mix_adc_dac[2][3] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc3_dac0 :
    mix_adc_dac[3][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc3_dac1 :
    mix_adc_dac[3][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc3_dac2 :
    mix_adc_dac[3][2] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_adc3_dac3 :
    mix_adc_dac[3][3] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del0_dac0 :
    mix_del_dac[0][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del0_dac1 :
    mix_del_dac[0][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del0_dac2 :
    mix_del_dac[0][2] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del0_dac3 :
    mix_del_dac[0][3] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del1_dac0 :
    mix_del_dac[1][0] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del1_dac1 :
    mix_del_dac[1][1] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del1_dac2 :
    mix_del_dac[1][2] = FIX16_FRACT_TRUNC(v.fix);
    break;
  case eParam_del1_dac3 :
    mix_del_dac[1][3] = FIX16_FRACT_TRUNC(v.fix);
    break;

  default:
    break;
  }
}

// EOF


