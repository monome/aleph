////////

//linFreq = powf(10.0f, (dspParameter[kFreq] * 3.0f) + 1.30103f)/getSampleRate();

a = Array.series(101, 0, 0.01);
b = a.collect({ arg x; 10.0 ** ((x*3.0) + 1.30103) });
b.plot;


////////


////exponential decay
x = 1.0;
y = 1.0;
n = 512;
m = -80.dbamp;
a = m ** (1 / n);
b = 1 / a;
q = 1 / (n-1);

//// curves
~expc = Array.series(9, -1.0, 0.25);

~exps = ~expc.collect({ arg c, i;
	var arr;
	("curve: "++c).postln;
	arr = Array.newClear(n);
	//	y = (n-j + 1) / n;
	y = 1.0;
	if(c >= 0.0, {
		x = 1.0;
		n.do({ arg j; 
			y = y - q;
			x = x * a;
			arr[j] = (y*(1-c)) + (x*c);
		});
	}, {
		x = m;
		n.do({ arg j; 
			y = y - q;
			x = x * b;
			arr[j] = ( y*(1-c.abs) ) + ( (1-x)*c.abs );
		});	
	});
	arr
});

~exps.plot;


/////// polynomial segments
// curves
~polc = Array.fill(12, { arg i; 1 / ((i / 4) + 1) });
~polys = ~polc.collect({ arg c, i;
	Array.fill(n, { arg j;
		(j / n) ** c;
	});
});

~polys.plot(~polc.asString);



/*

	0.00001.ampdb

	0.0625.ampdb

*/

