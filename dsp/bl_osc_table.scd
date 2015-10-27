
// base frequency: concert middle c, minus 3 octaves (~32hz)
~minhz = 60.midicps / 8;
// top frequency: concert middle c, plus 6 octaves (~16khz)
~maxhz = 60.midicps * 64;
// number of octaves
// -1 because we just use sine table for the top one always
~oct = log2(~maxhz / ~minhz) - 1;

// samplerate
~sr = 48000;

// table size
n = 2048;

// sine table
~sinetab = Array.fill(n, {|i| sin(i / n * 2 * pi) });
~sinetab.plot;

// bandlimited saw tables
// each octave can be half the size of the previous octave.
~sawtab = Array.fill(~oct, { |o| 
	Array.fill(n * (2 ** (-1 * o)), {0});
});

~oct.do({ arg o;
	// max hz in this octave
	var hihz = ~minhz * (2 ** (o+1));

	// number of harmonics.
	// don't need the top one because that will be just a sine table.
	~nharm = ((~sr * 0.5) / hihz).floor.asInteger;

	~nharm.do({ arg h;
		var m = ~sawtab[o].size;
		var amp = 1.0 / (h+1);
		var inc = (1.0 / m) * 2 * pi * (h+1);
		var x = 0;
		m.do({ arg i;
			~sawtab[o][i] = ~sawtab[o][i] + (sin(x) * amp);
			x = x + inc;
		});	
	});
});

~sawtab.plot(discrete:true);

// bandlimited triangle tables
// each octave can be half the size of the previous octave.
~tritab = Array.fill(~oct, { |o| 
	Array.fill(n * (2 ** (-1 * o)), {0});
});

~oct.do({ arg o;
	// max hz in this octave
	var hihz = ~minhz * (2 ** (o+1));

	// number of harmonics.
	// don't need the top one because that will be just a sine table.
	~nharm = ((~sr * 0.5) / hihz).floor.asInteger;

	~nharm.do({ arg h;
		var m = ~tritab[o].size;
		var amp = if(h % 3 == 0, {
			if((h / 3) % 2 == 0, {
				1.0 / ((h+1)**2)
			}, {
				-1.0 / ((h+1)**2)
			})
		}, { 0 });
		var inc = (1.0 / m) * 2 * pi * (h+1);
		var x = 0;
		m.do({ arg i;
			~tritab[o][i] = ~tritab[o][i] + (sin(x) * amp);
			x = x + inc;
		});	
	});
});

~tritab.plot(discrete:true);


// total points required:
~numpoints = ~sinetab.size;
~sawtab.do({ arg tab; ~numpoints = ~numpoints + tab.size; });
~tritab.do({ arg tab; ~numpoints = ~numpoints + tab.size; });
~numpoints * 4;