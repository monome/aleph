
// array of parameter descriptors
// 1st element is enum symbol
// 2nd element is unit symbol
// 3rd element is min
// 4th element is max
// numerical values in float
~params = [

	//---- -delay parameters

	// delay time - distance between read/write heads when synchronized
	[	\eParam_delay0, \s, 0, 30 ],
	// loop time - the size of the virutal tape loop
	[	\eParam_loop0, \s, 0, 30 ],
	// rate of read phasor
	[	\eParam_rate0, \u, 0, 1 ],
	// write level
	[	\eParam_write0, \u, 0, 1 ],
	// erase level (0 == SOS)
	[	\eParam_erase0, \u, 0, 1 ],
	// pos_write - directly set write head position
	[	\eParam_pos_write0, \s, 0, 30 ],
	// pos_read - directly set read head position
	[	\eParam_pos_read0, \s, 0, 30 ],

	[	\eParam_delay1, \s, 0, 30 ],
	[	\eParam_loop1, \s, 0, 30 ],
	[	\eParam_rate1, \u, 0, 1 ],
	[	\eParam_write1, \u, 0, 1 ],
	[	\eParam_erase1, \u, 0, 1 ],
	[	\eParam_pos_write1, \s, 0, 30 ],
	[	\eParam_pos_read1, \s, 0, 30 ],

	/*
	[	\eParam_delay2, \s, 0, 30 ],
	[	\eParam_loop2, \s, 0, 30 ],
	[	\eParam_rate2, \u, 0, 1 ],
	[	\eParam_write2, \u, 0, 1 ],
	[	\eParam_erase2, \u, 0, 1 ],
	[	\eParam_pos_write2, \s, 0, 30 ],
	[	\eParam_pos_read2, \s, 0, 30 ],

	[	\eParam_delay3, \s, 0, 30 ],
	[	\eParam_loop3, \s, 0, 30 ],
	[	\eParam_rate3, \u, 0, 1 ],
	[	\eParam_write3, \u, 0, 1 ],
	[	\eParam_erase3, \u, 0, 1 ],
	[	\eParam_pos_write3, \s, 0, 30 ],
	[	\eParam_pos_read3, \s, 0, 30 ],
	*/
	// SVF parameters
	[	\eParam_hz0, \hz, 0,  8000 ],
	[	\eParam_rq0, \u, 0, 1 ],
	[	\eParam_low0, \u, 0, 1 ],
	[	\eParam_high0, \u, 0, 1 ],
	[	\eParam_band0, \u, 0, 1 ],
	[	\eParam_notch0, \u, 0, 1 ],

	[	\eParam_hz1, \hz, 0,  8000 ],
	[	\eParam_rq1, \u, 0, 1 ],
	[	\eParam_low1, \u, 0, 1 ],
	[	\eParam_high1, \u, 0, 1 ],
	[	\eParam_band1, \u, 0, 1 ],
	[	\eParam_notch1, \u, 0, 1 ],

	//	[	\eParam_hz2, \hz, 0,  8000 ],
	//	[	\eParam_rq2, \u, 0, 1 ],
	//	[	\eParam_low2, \u, 0, 1 ],
	//	[	\eParam_high2, \u, 0, 1 ],
	//	[	\eParam_band2, \u, 0, 1 ],
	//	[	\eParam_notch2, \u, 0, 1 ],

	//	[	\eParam_hz3, \hz, 0,  8000 ],
	//	[	\eParam_rq3, \u, 0, 1 ],
	//	[	\eParam_low3, \u, 0, 1 ],
	//	[	\eParam_high3, \u, 0, 1 ],
	//	[	\eParam_band3, \u, 0, 1 ],
	//	[	\eParam_notch3, \u, 0, 1 ],

	//--- mix points:
	
	// in_delay
	[	\eParam_adc0_del0, \u, 0, 1 ],
	[	\eParam_adc0_del1, \u, 0, 1 ],
	[	\eParam_adc1_del0, \u, 0, 1 ],
	[	\eParam_adc1_del1, \u, 0, 1 ],
	[	\eParam_adc2_del0, \u, 0, 1 ],
	[	\eParam_adc2_del1, \u, 0, 1 ],
	[	\eParam_adc3_del0, \u, 0, 1 ],
	[	\eParam_adc3_del1, \u, 0, 1 ],

	//	[	\eParam_adc1_del1, \u, 0, 1 ],
	//	[	\eParam_adc2_del2, \u, 0, 1 ],
	//	[	\eParam_adc3_del3, \u, 0, 1 ],

	// in_svf
	//	[	\eParam_adc0_svf0, \u, 0, 1 ],
	//	[	\eParam_adc1_svf1, \u, 0, 1 ],
	//	[	\eParam_adc2_svf2, \u, 0, 1 ],
	//	[	\eParam_adc3_svf3, \u, 0, 1 ],
	
	// svf_delay
	//	[	\eParam_svf0_del0, \u, 0, 1 ],
	//	[	\eParam_svf1_del1, \u, 0, 1 ],
	//	[	\eParam_svf2_del2, \u, 0, 1 ],
	//	[	\eParam_svf3_del3, \u, 0, 1 ],

	// delay_svf
	//	[	\eParam_del0_svf0, \u, 0, 1 ],
	//	[	\eParam_del1_svf1, \u, 0, 1 ],
	//	[	\eParam_del2_svf2, \u, 0, 1 ],
	//	[	\eParam_del3_svf3, \u, 0, 1 ],

	// svf_out
	//	[	\eParam_svf0_dac0, \u, 0, 1 ],
	//	[	\eParam_svf1_dac1, \u, 0, 1 ],
	//	[	\eParam_svf2_dac2, \u, 0, 1 ],
	//	[	\eParam_svf3_dac3, \u, 0, 1 ],

		// delay_delay
	[	\eParam_del0_del0, \u, 0, 1 ],
	[	\eParam_del0_del1, \u, 0, 1 ],
	[	\eParam_del1_del0, \u, 0, 1 ],
	[	\eParam_del1_del1, \u, 0, 1 ],

	//	[	\eParam_del1_del0, \u, 0, 1 ],
	//	[	\eParam_del1_del1, \u, 0, 1 ],
	//	[	\eParam_del1_del2, \u, 0, 1 ],
	//	[	\eParam_del1_del3, \u, 0, 1 ],

	//	[	\eParam_del2_del0, \u, 0, 1 ],
	//	[	\eParam_del2_del1, \u, 0, 1 ],
	//	[	\eParam_del2_del2, \u, 0, 1 ],
	//	[	\eParam_del2_del3, \u, 0, 1 ],

	//	[	\eParam_del3_del0, \u, 0, 1 ],
	//	[	\eParam_del3_del1, \u, 0, 1 ],
	//	[	\eParam_del3_del2, \u, 0, 1 ],
	//	[	\eParam_del3_del3, \u, 0, 1 ],

	// svf_svf
	//	[	\eParam_svf0_svf0, \u, 0, 1 ],
	//	[	\eParam_svf0_svf1, \u, 0, 1 ],
	//	[	\eParam_svf0_svf2, \u, 0, 1 ],
	//	[	\eParam_svf0_svf3, \u, 0, 1 ],

	//	[	\eParam_svf1_svf0, \u, 0, 1 ],
	//	[	\eParam_svf1_svf1, \u, 0, 1 ],
	//	[	\eParam_svf1_svf2, \u, 0, 1 ],
	//	[	\eParam_svf1_svf3, \u, 0, 1 ],

	//	[	\eParam_svf2_svf0, \u, 0, 1 ],
	//	[	\eParam_svf2_svf1, \u, 0, 1 ],
	//	[	\eParam_svf2_svf2, \u, 0, 1 ],
	//	[	\eParam_svf2_svf3, \u, 0, 1 ],

	//	[	\eParam_svf3_svf0, \u, 0, 1 ],
	//	[	\eParam_svf3_svf1, \u, 0, 1 ],
	//	[	\eParam_svf3_svf2, \u, 0, 1 ],
	//	[	\eParam_svf3_svf3, \u, 0, 1 ],

	//-- mix/pan:
	// dry inputs _ outs
	[	\eParam_adc0_dac0, \u, 0, 1 ],
	[	\eParam_adc0_dac1, \u, 0, 1 ],
	[	\eParam_adc0_dac2, \u, 0, 1 ],
	[	\eParam_adc0_dac3, \u, 0, 1 ],

	[	\eParam_adc1_dac0, \u, 0, 1 ],
	[	\eParam_adc1_dac1, \u, 0, 1 ],
	[	\eParam_adc1_dac2, \u, 0, 1 ],
	[	\eParam_adc1_dac3, \u, 0, 1 ],

	[	\eParam_adc2_dac0, \u, 0, 1 ],
	[	\eParam_adc2_dac1, \u, 0, 1 ],
	[	\eParam_adc2_dac2, \u, 0, 1 ],
	[	\eParam_adc2_dac3, \u, 0, 1 ],

	[	\eParam_adc3_dac0, \u, 0, 1 ],
	[	\eParam_adc3_dac1, \u, 0, 1 ],
	[	\eParam_adc3_dac2, \u, 0, 1 ],
	[	\eParam_adc3_dac3, \u, 0, 1 ],

	// delays _ outs
	[	\eParam_del0_dac0, \u, 0, 1 ],
	[	\eParam_del0_dac1, \u, 0, 1 ],
	[	\eParam_del0_dac2, \u, 0, 1 ],
	[	\eParam_del0_dac3, \u, 0, 1 ],

	[	\eParam_del1_dac0, \u, 0, 1 ],
	[	\eParam_del1_dac1, \u, 0, 1 ],
	[	\eParam_del1_dac2, \u, 0, 1 ],
	[	\eParam_del1_dac3, \u, 0, 1 ],

	//	[	\eParam_del2_dac0, \u, 0, 1 ],
	//	[	\eParam_del2_dac1, \u, 0, 1 ],
	//	[	\eParam_del2_dac2, \u, 0, 1 ],
	//	[	\eParam_del2_dac3, \u, 0, 1 ],

	//	[	\eParam_del3_dac0, \u, 0, 1 ],
	//	[	\eParam_del3_dac1, \u, 0, 1 ],
	//	[	\eParam_del3_dac2, \u, 0, 1 ],
	//	[	\eParam_del3_dac3, \u, 0, 1 ],

];


// parameter header
~params_h = File(Document.current.dir ++ "/params.h", "w");
~params_h.write(" // autogenerated from param_gen.scd
#ifndef _ALEPH_LINES_PARAMS_H_
#define _ALEPH_LINES_PARAMS_H_

enum params {
");

~params.do({ arg desc, i;
	var str = desc[0].asString;
	~params_h.write(str);
	~params_h.write(",
");
});

~params_h.write("
eParamNumParams
};  

extern void fill_param_desc(void);

#endif // header guard 
// EOF");

~params_h.close;


// parameter source
~params_c = File(Document.current.dir ++ "/params.c", "w");
~params_c.write(" 
// autogenerated from param_gen.scd 

#include <string.h>
#include \"module.h\"
#include \"params.h\" 

void fill_param_desc(void) {
");
~params.do({ arg desc, i;
	var sym, label, unit, min, max;
	sym = desc[0];
	label = sym.asString[7..];
	unit = desc[1].asString;
	min = "0x"++(desc[2] << 16).asHexString;
	max = "0x"++(desc[3] << 16).asHexString;
	~params_c.write("strcpy(gModuleData->paramDesc[" ++ sym ++ "].label, \"" ++ label);
	~params_c.write("\");
");
	~params_c.write("strcpy(gModuleData->paramDesc[" ++ sym ++ "].unit, \"" ++ unit);
	~params_c.write("\");
");

	~params_c.write("gModuleData->paramDesc[" ++ sym ++ "].type = eParamTypeFix; 
");
	~params_c.write("gModuleData->paramDesc[" ++ sym ++ "].min = " ++ min ++ "; 
");
	~params_c.write("gModuleData->paramDesc[" ++ sym ++ "].max = " ++ max ++ "; 

");
});
~params_c.write("
}
// EOF");
~params_c.close;


// switch statement for setting params
~param_set_c = File(Document.current.dir ++ "/param_set_gen.c", "w");
~param_set_c.write(" 
// autogenerated from param_gen.scd 

#include \"params.h\" 
void module_set_param(u32 idx, pval v) {
  switch(idx) {
");
~params.do({ arg desc, i;
	var sym, label, unit, min, max;
	sym = desc[0];
	label = sym.asString[7..];
	unit = desc[1].asString;
	min = "0x"++(desc[2] << 16).asHexString;
	max = "0x"++(desc[3] << 16).asHexString;
	~param_set_c.write("case " ++ sym ++ " :
");

	/*
if(v.fix < "++min++") {
  v.fix = "++min++";
}
if(v.fix < "++max++") {
  v.fix =" ++max++";
}
	*/

	[i, desc].postln;
	
if(i > 25, {
	if(false, { //i > 71, {
			var sym, str;
		sym = desc[0];
		str = sym.asString[7..];
		str = str.select({ |ch| (ch != $0) &&  (ch != $1) &&  (ch != $2) &&  (ch != $3) });
		~param_set_c.write("    mix_" ++ str ++ "["++(((i-72)%16)/4).floor ++"][" ++ (i%4) ++ "] = v.fix;
    ");
	}, {
		var sym, str;
		sym = desc[0];
		str = sym.asString[7..];
		str = str.select({ |ch| (ch != $0) &&  (ch != $1) &&  (ch != $2) &&  (ch != $3) });
		~param_set_c.write("    mix_" ++ str ++ "[" ++ (i%4) ++ "] = pval.fix;
    ");
	});
	
}, {
	~param_set_c.write("//   ...
    ");
});

~param_set_c.write(" break;
");
	
});
~param_set_c.write("
  default:
  break;
  }
}

// EOF");
~param_set_c.close;


postln(" number of parameters: " ++ ~params.size );


/*
z = "balls";
z[2].inspect
z.select({ arg ch; ch.postln; ch != $l });
*/

(12 / 4).floor