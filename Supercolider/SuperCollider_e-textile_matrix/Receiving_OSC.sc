s.boot;

// Array to mapp \vowel
(
a = Array2D.fromArray(16,16,
	[
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0,
	0,1,2,3,4,0,1,2,3,4,0,1,2,3,4,0
	]
);
)

// Array to mapp \note
(
b = Array2D.fromArray(16,16,
	[
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	]
);
)

(
g = Synth.head(s, \sillyChoir, [\amp, 0]);
h = Synth.head(s, \sillyChoir, [\amp, 0]);
i = Synth.head(s, \sillyChoir, [\amp, 0]);
j = Synth.head(s, \sillyChoir, [\amp, 0]);
k = Synth.head(s, \sillyChoir, [\amp, 0]);
l = Synth.head(s, \sillyChoir, [\amp, 0]);
m = Synth.head(s, \sillyChoir, [\amp, 0]);
n = Synth.head(s, \sillyChoir, [\amp, 0]);
)

o = List[ g, h, i, j, k, l, m, n ];

(
p = OSCFunc(
	{
		arg msg, time, addr;
		var row, col;
		var lastState = 1.0;

		msg.drop(1).clump(3).do { |triplet|
			triplet.postln;

		if ( triplet[1] > -1 ,{
			// triplet.postln;
			col = triplet[2] % 16;
			row = (triplet[2] / 16).floor;
			////////// ON
			o.wrapAt(triplet[0]).set( \vowel, a[col,row] );
			o.wrapAt(triplet[0]).set( \note, b[col,row] );
			o.wrapAt(triplet[0]).set( \amp, triplet[1] / 200 );
			// o.wrapAt(triplet[0]).set( \gate, 1 );
			}, {
			////////// OFF
			o.wrapAt(triplet[0]).set( \amp, 0 );
			// o.wrapAt(i).set( \gate, 0 );
			});
		}
	 }, '/sensor', nil , 7771 );
)

p.free; // remove the OSCFunc when you are done.
s.quit;
