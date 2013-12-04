// 1pole integtrator coefficient scaling table

// 10-bit linear input
n = 2 ** 10;
i = Array.series(n);

/// sample rate
~sr = 48000.0;
// coefficienct function given  decay time
~fc = { arg t; exp(-2.0 * (pi / (t * ~sr))) };
// decay time function given coefficient
~ft = { arg c; (-2.0 * pi) / (log(c) * ~sr) };

// find max decay time given numerical limitations of 1.31 coefficient
// max value of coeff:
~maxc = 0x7fffffff.asFloat / (0x7fffffff.asFloat + 1.0);

// what if we use 1.15 coefficient?
~maxc = 0x7fff.asFloat / (0x7fff.asFloat + 1.0);
// this gives a little > 4.2 seconds...
// so probably inadequate at audio rate,
// would be fine with a control rate of 1khz or so.

// let's use 32-bit coefficient at audio rate,
// may want to reduce it in the future.

// max time:
~maxt = ~ft.value(~maxc);
// with 32-bit coefficient this is ridiculously large, so let's choose something arbitrary.
// 64 seconds seems like more than enough.

~maxt = 64.0;
~mint = 0.0;

// exponentiation ?
//~pow = 2.0;
// nah
~pow = 1.0;

// power series
/*
f = i.collect({ |j|
	var x = j.linlin(0, n-1, 0.0, 1.0);
	x = x ** ~pow;
	x = x.linlin(0.0, 1.0, ~mint, ~maxt);
	x
});
*/

// logarithmic
f = i.collect({ |j|
	var x = j.linexp(0, n-1, 1.0, 1000.0) - 1.0;
	x = x.linlin(0.0, 999.0, ~mint, ~maxt);
	x 
});
f.plot;
f.do({ |x| x.postln; });

// compute integration coefficient from time
v = f.collect({ |x| (~fc.value(x) * 0x7fffffff).asInteger });
v.do({ arg y; y.asHexString.postln; });

// compute 16.16 representation
r = f.collect({ |x, i|
	var xint, xfr, y;

	xint = x.floor;
	xfr = x - xint;
	xint = xint.asInteger;

	y = (xint << 16) | (0xffff & ((xfr * 0x10000).floor.asInteger));
});
r.do({ arg y; y.asHexString.postln; });



////// output to files
/// binary, big endian

// value
~vf = File.open(File.getcwd ++ "/scaler_integrator_val.dat", "wb");
// write size
~vf.putInt32(n);
// write data
v.do({ |x| ~vf.putInt32(x) });
~vf.close;

// representation
~rf = File.open(File.getcwd ++ "/scaler_integrator_rep.dat", "wb");
// write size
~rf.putInt32(n);
// write data
r.do({ |x| ~rf.putInt32(x) });
~rf.close;