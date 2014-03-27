Routine {


n = 1024;
b = Buffer.alloc(s, n, 1);
0.25.wait;
// print
p = { 
	b.getToFloatArray(wait:0.25, action:{|a, j|

		{ a.plot(j.asString) }.defer;

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

// some odd harmonics
postln(" // 1: odd : ");
postln(" { ");
b.sine1([1, 0, 1/3, 0, 1/5], asWavetable:false);
0.25.wait;
p.value;
"} , ".postln;
"".postln;

// some even harmonics
postln(" // 2: even : ");
postln(" { ");
b.sine1([1, 1/2, 0, 1/4, 0, 1/6], asWavetable:false);
0.25.wait;
p.value;
"} , ".postln;
"".postln;

// even + odd
postln(" // 3: even + odd, order 7 : ");
postln(" { ");
b.sine1([1, 1/2, 1/3, 1/4, 1/5, 1/6, 1/7], asWavetable:false);
0.25.wait;
p.value;
"} , ".postln;

// higher order
postln(" // 3: even + odd, order 11 : ");
postln(" { ");
b.sine1([1, 1/2, 1/3, 1/4, 1/5, 1/6, 1/7, 1/8, 1/9, 1/10, 1/11], asWavetable:false);
0.25.wait;
p.value;
"} , ".postln;



}.play;