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

(
SynthDef("pulse", {
	var osc1, osc2, peg, duty;
	
	// pitch envelope
	peg = XLine.kr(3050, 50, 3);
	
	// make a normal sawtooth
	osc1 = Saw.ar(peg).range(-1,1); // a sawtooth wave
	
	// duty = 0..period where (period*0.5) = 50% duty cycle
	// modulate duty with a sine wave
	duty = (1/peg) * SinOsc.kr(0.23).range(0.1,0.9);
	
	// invert original sawtooth and delay it
	osc2 = DelayC.ar(neg(osc1), 1, duty);
	
	Out.ar([0,1], osc1 + osc2); // add the two saws together
	}).send(s);
)

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