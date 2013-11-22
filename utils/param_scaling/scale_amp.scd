// amplitude scaling table

// 10-bit linear input
n = 2**10;
(1.0 / n).postln;
i = Array.series(n);

// table for values
// type is 1.32
// [0..0x7ffffff] is used normally,
// 0xffffffff has the special meaning of unity gain. 

/// minimum value in db for logarithmic curve
~mindb = -48;

/// float values for output
f = i.collect({ |j|
	if(j ==0, {
		0.0
	}, {
		if(j == (n-1), { 
			1.0
			//			-1.0
		}, {
			j.linlin(1, n-1, ~mindb, 0).dbamp;
		})
	})
	
});
//f.plot("log");

// find a breakpoint based on slope, linear-ize below it 
/// FIXME: would be better to match the slope with a polynomial interpolation.
/// but this will certainly do for now.
~target = 0.25 / n; // target slope
b = block { |break|
	var d;
	var j = 0;
	f.doAdjacentPairs({
		arg y0, y1;
		if(j == 0, {
			// skip first point
		}, {
			// break when slope > 1
			d = (y1 - y0);
			if(d > ~target, { break.value( j+1 ) });
		});
		j = j + 1;
		
	});
};

// this is the index of the inflection point
 b.postln;

// linear fill
~bval = f[b];
b.do({ arg j;
	f[j] = j.linlin(0, b, 0, ~bval);
});


//// table for output values
/// type is 1.31, unit is amplitude
v = f.collect({ |x| 
	(x * 0x7fffffff).asInteger;
});

// table for UI representation
// type is 16.16, unit is decibels
r = v.collect({ |x|
	var db, dbint, dbfr;
	db = (x.asFloat / (0x7fffffff.asFloat + 1.0)).ampdb;
	db.postln;
	dbint = db.floor.asInteger;
	dbfr = db - (dbint.asFloat);
	if(dbint == -inf, {
		0xffffffff
	}, {
		(dbint << 15) | ((dbfr * 0x10000).floor.asInteger);
	});
});

r.do({ |x| x.asHexString.postln; });

////// output to files
/// binary, little endian

// value
~vf = File.open(File.getcwd ++ "/scaler_amp_val.bin", "wb");
// write size
~vf.putInt32(n);
// write data
v.do({ |x| ~vf.putInt32(x) });
~vf.close;

// representation
~rf = File.open(File.getcwd ++ "/scaler_amp_rep.bin", "wb");
// write size
~rf.putInt32(n);
// write data
r.do({ |x| ~rf.putInt32(x) });
~rf.close;