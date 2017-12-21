This is the recipe to make an arbitrary precision fract param which
displays as you want in BEES:

referring to an example of a 3.12 fract 'timescale' type for lines in
this explanation.  It should display the numbers 0.000 - 4.000 in
response to BEES bangs from 0 - 0x4000

1. write the fract type representation of your param.  e.g 3.12
2. use eParamTypeFix desc[eParamTimescale].type = eParamTypeFix;
3. set .radix quantity as leading number of representation + 1  e.g we have 3.12, so take 3 & add 1 giving  desc[eParamTimescale].radix = 4;
4. set .max to be the max BEES bang << .radix.  e.g   desc[eParamTimescale].max = 0x00040000; (we get the number by doing 0x4000 << 4)
5. set .min the same as .max
6. shift the param right by (.radix + 1) to recover the 16 bit BEES number on blackfin e.g globalTimescale = v >> 5;
7. shift the desired 16-bit param left by .radix when doing param_setup e.g - we want to initialise to 1.0 in 3.12 (1 << 12) - shifting that left 4 gives param_setup(eParamTimescale,  1 << 16);

so overall we have:

Param descriptor:

  strcpy(desc[eParamTimescale].label, "timescale");
  desc[eParamTimescale].type = eParamTypeFix;
  desc[eParamTimescale].min = 0;
  desc[eParamTimescale].max = 0x00040000;
  desc[eParamTimescale].radix = 4;

Module param setup :
  param_setup(eParamTimescale,  1 << 16);// 16 == 4 + 12

Module param receive
  case eParamTimescale :
    globalTimescale = v >> 5; // This part seems weird!
    //    filter_ramp_tog_set_inc(&(lpFadeWr[1]), v);
    break;


