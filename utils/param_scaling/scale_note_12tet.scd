// note scaling table
// 12-tone equal temperament, with microtunes

// 10-bit linear input gives 7 microtunes, costs 8k
n = 2**10;

// 12-bit linear input gives 31 microtunes, costs 32k
// n = 2**12;

i = Array.series(n);


// let's use 128 semitones.
// so the full range is 10 octaves + 8 semitones.

// number of semitones
~nsemi = 128;

// microtuned steps per semitone
~nmicro = n / ~nsemi;

// constant 12th root of 2
~root_semi = 2 ** (1/12);
// constant 32 root of tet
~root_micro = ~root_semi ** (1/~nmicro);

// semitone ratios
~fsemi = ~nsemi.collect({
	|j| (2 ** ( j / 12).floor) 
	* (~root_semi ** (j % 12))
 });

// ~fsemi.plot;

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

//f.do({ |j, x| if(x.isNil, { "PHFPOH!POPW!OH!W".postln; }) }); 

// base frequency: concert middle c, minus 5 octaves (~8hz)
~base = 60.midicps / 32;
~base.postln;
	
// multiply float ratio table
f = f * ~base;

f.plot;

f.do({ |x, j|
	(" " ++ j ++ " : " ++ x ++ " hz : " ++ x.cpsmidi).post; 
	if( (x.cpsmidi - (x.cpsmidi.floor )) < 0.0001, {
		"     <<<<---------------------------"
	}, { ""
	}).postln;
});

/// write to files

// get human-readable representation

// float-to-fix16 function
~float_fix16  = { arg x;
	var xin, xfr;
	xin = x.floor.asInteger;
	xfr = 
};

// get unit frequency
