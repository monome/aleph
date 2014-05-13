//ADD pulse 50%
/*
 open in PD
 s.boot
 select all
 cmd+return
 copy output to wavtab_data_inc.c
 
 PS for variable duty cycle pulse wave, this may be useful:
 
 d = duty cycle ratio (0-1)
 
 a = amplitude of nth harmonic = 2 / (pi * n) * sin(n * pi * d)
 
 (so square wave is a special case; d = 0.5 so the sine term is zero for even harmonics.)
 
 sum as many harmonics as you like to achieve desired richness / risk of aliasing.
 */



Routine {

n = 4096;
b = Buffer.alloc(s, n, 1);
0.25.wait;
// print
p = {
	b.getToFloatArray(wait:0.25, action:{|a|

		//		{ a.plot }.defer;

		a.do({ |x, i|
			"0x".post;
			(x * 0x7fffffff).floor.asInteger.asHexString.post;
			", ".post;
		});
	});
};

// sine
postln(" // 0: sine : ");
postln(" { ");
b.sine1([1], asWavetable:false);
0.25.wait;
p.value;
" } , ".postln;
"".postln;

}.play;