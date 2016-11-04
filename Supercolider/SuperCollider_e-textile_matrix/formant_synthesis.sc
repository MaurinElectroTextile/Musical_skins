s.boot;
// Control rate so as not to whack your speakers with DC
{ Gate.kr(WhiteNoise.kr(1, 0), LFPulse.kr(1.333, 0.5))}.scope(zoom: 20);

(
// SynthDef
SynthDef(\sillyChoir, { arg
	freq = 434,
	amp = 0.5,
	vibratoSpeed = 5,
	vibratoDepth = 4,
	vowel = 0,
	att = 0.01,
	rel = 1.5,
	lag = 0.5,
	gate = 1;

	var in, vibrato, env, va, ve, vi, vo, vu, snd, temp_freq, temp_lag, temp_vowel;

  // env = EnvGen.kr(Env.asr(att, 1, rel), gate, doneAction: 2);
  env = EnvGen.kr(Env.asr(att, 1, rel), gate, doneAction: 2);
  snd = DC.ar(0) ! 2;

  5.do { |i|
    vibrato = SinOsc.kr(vibratoSpeed + LFNoise1.kr(1).range(-1, 1), mul: vibratoDepth * LFNoise1.kr(1).range(0.5, 1));

    temp_freq = DelayC.kr(freq, 0.2, LFNoise1.kr(1).range(0, 0.2)) * LFNoise1.kr(0.5).range(0.98, 1.02);
    temp_lag = lag * LFNoise1.kr(1).range(0.5, 1);
	// temp_lag = 0;
    temp_vowel = DelayC.kr(vowel, 0.2, LFNoise1.kr(1).range(0, 0.2));

    in = VarSaw.ar(Lag.kr(temp_freq, temp_lag) + vibrato, width: LFNoise1.kr(1).range(0.05, 0.2)) * LFNoise1.kr(1).range(0.5, 1);


    va = BBandPass.ar(
      in: in,
      freq: [ 600, 1040, 2250, 2450, 2750 ],
      bw: [ 0.1, 0.067307692307692, 0.048888888888889, 0.048979591836735, 0.047272727272727 ],
      mul: [ 1, 0.44668359215096, 0.35481338923358, 0.35481338923358, 0.1 ]);

    ve = BBandPass.ar(
      in: in,
      freq: [ 400, 1620, 2400, 2800, 3100 ] ,
      bw: [ 0.1, 0.049382716049383, 0.041666666666667, 0.042857142857143, 0.038709677419355 ],
      mul: [ 1, 0.25118864315096, 0.35481338923358, 0.25118864315096, 0.12589254117942 ]);

    vi = BBandPass.ar(
      in: in,
      freq: [ 250, 1750, 2600, 3050, 3340 ] ,
      bw: [ 0.24, 0.051428571428571, 0.038461538461538, 0.039344262295082, 0.035928143712575 ],
      mul: [ 1, 0.031622776601684, 0.15848931924611, 0.079432823472428, 0.03981071705535 ] );

    vo = BBandPass.ar(
      in: in,
      freq:[ 400, 750, 2400, 2600, 2900 ] ,
      bw: [ 0.1, 0.10666666666667, 0.041666666666667, 0.046153846153846, 0.041379310344828 ],
      mul: [ 1, 0.28183829312645, 0.089125093813375, 0.1, 0.01 ]);

    vu = BBandPass.ar(
      in: in,
      freq: [ 350, 600, 2400, 2675, 2950 ],
      bw: [ 0.11428571428571, 0.13333333333333, 0.041666666666667, 0.044859813084112, 0.040677966101695 ],
      mul: [ 1, 0.1, 0.025118864315096, 0.03981071705535, 0.015848931924611 ]);

    snd = snd + Pan2.ar(SelectX.ar(Lag.kr(temp_vowel, temp_lag), [va, ve, vi, vo, vu]), i.linlin(0, 4, -1, 1));
  };

  snd = Compander.ar(snd, snd, -30.dbamp, 1, 0.5, 0.01, 0.1) * 6.dbamp;

  snd = FreeVerb2.ar(snd[0], snd[1], 0.5, 1, 1);
(((((
	Out.ar(0, snd * env * amp);
}).add;
)

(
p = Pmono(
	\sillyChoir,
	\note, 0,
	\amp, Pbrown(-12, -6, 0.1).dbamp,
	\vibratoSpeed, Pwhite(40, 10),
	\vibratoDepth, 2,
	\vowel, 4,
).play;
)
p.stop;

(
p = Pmono(
	\sillyChoir,
	\note, Pwrand([0, 2, 4, 5, 7, 9], [5, 2, 3, 3, 4, 1].normalizeSum, repeats:inf),
	// \note, Pwrand([-8, -4, -3, -1, 2, 3], [5, 2, 3, 3, 4, 1].normalizeSum, repeats:inf),
	\ctranspose, 5,
	\dur, Pstutter(Prand([1, 2, 4], repeats:inf), Pwrand([1, 0.5, 0.25], [1, 2, 1].normalizeSum, repeats:inf)),
	\amp, Pbrown(-12, -6, 0.1).dbamp,
	\vibratoSpeed, Pwhite(4, 5),
	\vowel, Pwrand([0, 1, 2, 3, 4], [4, 3, 2, 1, 1].normalizeSum, repeats:inf),
	\lag, 0.5,
	\att, 0.01,
).play;
)

p.free;

(
var a;
a = Pwrand([2, 3, 4], [1, 2, 3].normalizeSum, inf).asStream;
a.next;
)


s.quit



