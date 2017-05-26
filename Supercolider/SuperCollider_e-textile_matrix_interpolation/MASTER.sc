s.boot;

(

g = Synth.head(s, \sillyChoir, [\amp, 0, \ctranspose, 5, \lag, 0.5, \att, 0.01], \dur, 10);
h = Synth.head(s, \sillyChoir, [\amp, 0, \ctranspose, 5, \lag, 0.5, \att, 0.01], \dur, 10);
i = Synth.head(s, \sillyChoir, [\amp, 0, \ctranspose, 5, \lag, 0.5, \att, 0.01], \dur, 10);
j = Synth.head(s, \sillyChoir, [\amp, 0, \ctranspose, 5, \lag, 0.5, \att, 0.01], \dur, 10);
k = Synth.head(s, \sillyChoir, [\amp, 0, \ctranspose, 5, \lag, 0.5, \att, 0.01], \dur, 10);
l = Synth.head(s, \sillyChoir, [\amp, 0, \ctranspose, 5, \lag, 0.5, \att, 0.01], \dur, 10);
m = Synth.head(s, \sillyChoir, [\amp, 0, \ctranspose, 5, \lag, 0.5, \att, 0.01], \dur, 10);
n = Synth.head(s, \sillyChoir, [\amp, 0, \ctranspose, 5, \lag, 0.5, \att, 0.01], \dur, 10);

o = List[ g, h, i, j, k, l, m, n ];

)
o.free;

(
p = OSCFunc(
	{
		arg msg, time, addr;
		var posX, posY;

		msg.drop(1).clump(5).do { |packet|
			// $0 : blob ID
			// $1 : blob posX
			// $2 : blob posY
			// $3 : blob posZ
			// $4 : blob perimeter

		if ( packet[4] > 0 ,{
			packet.postln;
			posX = packet[1]/4; // 0-63 to 0-15
			posY = packet[2]/4;

			////////// ON
			o.wrapAt(packet[0]).set(
					\vowel, a[posX, posY],
					\note, b[posX, posY],
					\amp, packet[3]/255,
					\vibratoSpeed, Pwhite(4, 5));
			}, {
			////////// OFF
			o.wrapAt(packet[0]).set(\amp, 0);
			//o.wrapAt(packet[0]).set(\gate, 0);
			});
		}
	 }, '/sensors', nil , 7771);
)

p.free; // remove the OSCFunc when you are done.
s.quit;


\dur, Pstutter(Prand([1, 2, 4], repeats:inf), Pwrand([1, 0.5, 0.25], [1, 2, 1].normalizeSum, repeats:inf)),
\vowel, Pwrand([0, 1, 2, 3, 4], [4, 3, 2, 1, 1].normalizeSum, repeats:inf),



