//---------------------
//---- signal

// number of input points
n = 1024;
// sinewave input
x = Array.fill(n, { |i| sin  (i  / n * 2 * pi ) });
// number of distortion settings
m = 32;

//----------------------
//--- distortion: pulse->square
a = Array.fill(m, { |i| 
	var a;
	a = i.linlin(0, m-1, -1.0, 0.999);
	(2.0 * a) / ( 1.0 - a);
});

y = a.collect({ |k| x.collect({ |z|
	(1.0 + k) * z / (1.0 + (k * z.abs))
}) });

y.plot;

//------------------------
//--- soft clipping

a = Array.fill(m, { |i| 
	i.linlin(0, m-1, 0.0, 0.999);
});

y = a.collect({ |k| x.collect({ |z|

	var zs, za;
	zs = z.sign;
	za = z.abs;

	if(za <= k, { 
		v = z;
	}, {
		// f(x) = a + (x-a)/(1+((x-a)/(1-a))^2)
		var r, c, d;
		d = za - k;
		r = d / (1.0 - k);
		c = d / (1.0 + (r*r));
		v = k + c;
		v = v * zs;
	});	
	// normalize
	//	v = v * (1.0 / ((k + 1.0 ) / 2.0 ));
}) });

//y.do({ |q, j| q.plot ("a : " ++ a[j]); });
y.plot;