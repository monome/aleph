//input file just_data.txt
//output file scaler_note_just_val.dat

~jf = CSVFileReader.read("/Aleph/aleph-dev/modules/prgm/just_data.txt").postln;

n = 2**10;

i = Array.series(n);

i = ~jf.collect{ |j| j.asFloat };

~base = 11.875;
~base.postln;

i = i * ~base;

// convert to 16.16.
v = i.collect({ |x, j|
	var xint, xfr, y;

	xint = x.floor;
	xfr = x - xint;
	xint = xint.asInteger;

	y = (xint << 16) | (0xffff & ((xfr * 0x10000).asInteger));
});
//v.do({ arg y; y.asHexString.postln; });

////// output to files
/// binary, big endian

// value
~vf = File.open(File.getcwd ++ "/Aleph/aleph-dev/modules/prgm/scaler_note_just_val.dat", "wb");
// write size
~vf.putInt32(n);
// write data
v.do({ |x| ~vf.putInt32(x) });
~vf.close;
