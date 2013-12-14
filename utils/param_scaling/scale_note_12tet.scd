// note scaling table
// 12-tone equal temperament, with microtunes

// 10-bit linear input gives 7 microtunes, costs 4k
n = 2**10;

// 12-bit linear input gives 31 microtunes, costs 16k
// n = 2**12;


i = Array.series(n);

// let's use 128 semitones.
// so the full range is 10 octaves + 8 semitones.

// number of semitones
~nsemi = 128;

// microtuned steps per semitone
~nmicro = n / ~nsemi;

// semitone tempered ratio
~root_semi = 2 ** (1/12);
// microtune tempered ratio
~root_micro = ~root_semi ** (1/~nmicro);

// semitone ratios of fundamental
~fsemi = ~nsemi.collect({
	|j| (2 ** ( j / 12).floor) 
	* (~root_semi ** (j % 12))
 });

// fill in semitones on main table
// float ratios
f = i.collect({ |j| 
	if( (j % ~nmicro) == 0, {
		~fsemi[ (j / ~nmicro).floor ]
	}, { 
		nil
	});
});

// fill in microtunings
{
	var fSemId = 0;
	// 32nd root of ratio between semitones
	~nsemi.do({ arg iSem;
		~nmicro.do({
			arg iMic;
			if (iMic != 0, {
				f[fSemId + iMic] = 
				f[fSemId] * (~root_micro ** iMic)
			});
		});
		fSemId = fSemId + ~nmicro;
	});
}.value;

// base frequency: concert middle c, minus 5 octaves (~8hz !)
~base = 60.midicps / 32;
~base.postln;
	
// multiply float ratio table
f = f * ~base;
//f.plot;

f.do({ |x, j|
	(" " ++ j ++ " : " ++ x ++ " hz : " ++ x.cpsmidi).post; 
	if( (x.cpsmidi - (x.cpsmidi.floor )) < 0.0001, {
		"     <<<<---------------------------"
	}, { ""
	}).postln;
});

// convert to 16.16.
// fortunately, it doesn't cost much to use the 16.16 representation directly in the DSP.
// so i think we can get away with only one table.
v = f.collect({ |x, i|
	var xint, xfr, y;

	xint = x.floor;
	xfr = x - xint;
	xint = xint.asInteger;

	y = (xint << 16) | (0xffff & ((xfr * 0x10000).floor.asInteger));
});
v.do({ arg y; y.asHexString.postln; });

////// output to files
/// binary, big endian

// value
~vf = File.open(File.getcwd ++ "/scaler_note_12tet_val.dat", "wb");
// write size
~vf.putInt32(n);
// write data
v.do({ |x| ~vf.putInt32(x) });
~vf.close;

// use the same 16.16 values for SVF corner frequency representation.
// new table for corresponding coefficients.

// 2x oversampled
~sr = 96000.0;
~cf = f.collect({ |fc| 
	2.0 * sin(pi * min(0.25, fc / ~sr));
});
//~cf.do({ |x| x.postln; });
//~cf.plot;

~cfv = ~cf.collect({ |x| (x * 0x7fffffff).asInteger });
~cfv.do({ |x| x.postln; });
// value
~cfvf = File.open(File.getcwd ++ "/scaler_svf_fc_val.dat", "wb");
// write size
~cfvf.putInt32(n);
// write data
~cfv.do({ |x| ~cfvf.putInt32(x) });
~cfvf.close;