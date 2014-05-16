//input file: just_data.txt
//output file: scaler_note_just_val.dat

//importing ratios as text file
~jf = CSVFileReader.read("/Aleph/aleph-dev/modules/prgm/custom_data.txt");

//size of array 1024, 16 octaves of 64 just ratios
n = 2**10;

//create array
i = Array.series(n);

//copying items to a new sliced 1D array
i = ~jf.collect ({ |j| j.asFloat });
f = i.slice(nil, (0..1023));

//setting base frequency in Hz
~base = 190;
~base.postln;

//multiplying ratios with base frequency
f = f * ~base;

//converting to .dat file using the rest of the original script
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
~vf = File.open(File.getcwd ++ "/scaler_note_custom_val.dat", "wb");
// write size
~vf.putInt32(n);
// write data
v.do({ |x| ~vf.putInt32(x) });
~vf.close;
