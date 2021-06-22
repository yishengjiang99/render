
const genstrs = ["StartAddrOfs", "EndAddrOfs", "StartLoopAddrOfs", "EndLoopAddrOfs", "StartAddrCoarseOfs", "ModLFO2Pitch", "VibLFO2Pitch", "ModEnv2Pitch", "FilterFc", "FilterQ", "ModLFO2FilterFc", "ModEnv2FilterFc", "EndAddrCoarseOfs", "ModLFO2Vol", "Unused1", "ChorusSend", "ReverbSend", "Pan", "Unused2", "Unused3", "Unused4", "ModLFODelay", "ModLFOFreq", "VibLFODelay", "VibLFOFreq", "ModEnvDelay", "ModEnvAttack", "ModEnvHold", "ModEnvDecay", "ModEnvSustain", "ModEnvRelease", "Key2ModEnvHold", "Key2ModEnvDecay", "VolEnvDelay", "VolEnvAttack", "VolEnvHold", "VolEnvDecay", "VolEnvSustain", "VolEnvRelease", "Key2VolEnvHold", "Key2VolEnvDecay", "Instrument", "Reserved1", "KeyRange", "VelRange", "StartLoopAddrCoarseOfs", "Keynum", "Velocity", "Attenuation", "Reserved2", "EndLoopAddrCoarseOfs", "CoarseTune", "FineTune", "SampleId", "SampleModes", "Reserved3", "ScaleTune", "ExclusiveClass", "OverrideRootKey", "Dummy"];
const attrsym = genstrs.map(str => Symbol(str));

let rctx;
const pcmCache = new WeakMap();
document.body.onclick = async (e) => {
	if (e.target.classList.contains("fftbins")) {
		e.preventDefault();

		rctx = rctx || new AudioContext();
		const real = e.target.getAttribute("real").split(",");
		const imag = e.target.getAttribute("img").split(",");
		let ctx = new OfflineAudioContext({
			numberOfChannels: 1,
			length: 4096,
			sampleRate: 4096,
		});

		let osc = new OscillatorNode(ctx, {
			type: "custom",
			periodicWave: new PeriodicWave(ctx, {
				imag, real
			}),
			frequency: 1,
		});
		osc.connect(ctx.destination);
		osc.start();
		osc.stop(1.0);
		const ab = (await ctx.startRendering());
		const abs = new AudioBufferSourceNode(rctx, {buffer: ab, loop: true});
		abs.start(); abs.stop(1);
		const curve = ab.getChannelData(0);
		//	chart(curve, document.querySelector("canvas"));
		return;
	}
	if (e.target.classList.contains("attlist")) {
		e.preventDefault();

		const attrs = e.target.getAttribute("attrs").split(",");

		document.querySelector("#details").innerHTML = attrs.map((attr, index) => `${genstrs[index]}:${attr}`).join('<br>')
		return;
	}
	if (e.target.classList.contains('pcm')) {
		e.preventDefault();
		e.preventDefault();

		const sr = e.target.getAttribute('sr');
		const startloop = e.target.getAttribute("startloop");
		const endloop = e.target.getAttribute("endloop")
		const pitch = e.target.getAttribute("pitch");
		const file = e.target.getAttribute('file'); const mid = e.target.getAttribute('midi') || 55;

		const range = e.target.getAttribute('range');
		const attrs = e.target.parentElement.parentElement.querySelector(".attlist")
			.getAttribute("attrs").split(",");
		const zone = new Proxy(attrs, {
			get: (target, key) => genstrs.indexOf(key) > -1
				? parseInt(target[genstrs.indexOf(key)])
				: null
		})
		const rangeHeaders = {
			headers: {
				Range: range
			}
		};
		const cacheKey = rangeHeaders;
		if (!pcmCache.has(cacheKey)) {
			const pcm = await (await fetch(file, rangeHeaders)).arrayBuffer();
			pcmCache.set(cacheKey, pcm);
		}
		const ab = pcmCache.get(cacheKey)


		const s16tof32 = (i16) => i16 / 0xffff;
		const ctx = new AudioContext({sampleRate: sr});
		const s16s = new Int16Array(ab)
		const audb = ctx.createBuffer(2, s16s.length, sr);
		const buffer = audb.getChannelData(0);
		for (let i = 0;i < audb.length;i++) {
			buffer[i] = s16tof32(s16s[i]);
		}
		const abs = new AudioBufferSourceNode(ctx, {
			buffer: audb,
			loopStart: parseInt(startloop),
			loopEnd: parseInt(endloop),
			playbackRate: mid * 100 / parseFloat(pitch),
			loop: true
		});


		let lpf = new BiquadFilterNode(ctx,
			{
				frequency: Math.pow(2, zone.FilterFc / 1200) * 8.176,
				Q: zone.FilterQ / 10,
				type: "lowpass"
			});
		const modEnvelope = new GainNode(ctx, {gain: 0});

		modEnvelope.connect(lpf.detune);

		if (zone.ModEnvAttack + zone.ModEnvDelay > -12000) {

			modEnvelope.gain.linearRampToValueAtTime(1, Math.pow(2, (zone.ModEnvAttack) / 1200));
		} else {
			modEnvelope.gain.exponentialRampToValueAtTime(1, .001);
		}

		modEnvelope.gain.setTargetAtTime(1 - zone.ModEnvSustain / 1000, Math.pow(2, zone.ModEnvDecay) / 1200, 5);
		const volumeEnveope = new GainNode(ctx, {gain: 0});

		volumeEnveope.gain.linearRampToValueAtTime(
			Math.pow(10, -zone.Attenuation / 200),
			Math.pow(2, (zone.VolEnvAttack) / 1200));

		volumeEnveope.gain.setTargetAtTime(1 - zone.VolEnvSustain / 1000, Math.pow(2, zone.VolEnvDecay / 1200), .4);

		abs.connect(lpf).connect(volumeEnveope).connect(ctx.destination);
		abs.start();

		return false;
	}
}
