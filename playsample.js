const genstrs = ["StartAddrOfs", "EndAddrOfs", "StartLoopAddrOfs", "EndLoopAddrOfs", "StartAddrCoarseOfs", "ModLFO2Pitch", "VibLFO2Pitch", "ModEnv2Pitch", "FilterFc", "FilterQ", "ModLFO2FilterFc", "ModEnv2FilterFc", "EndAddrCoarseOfs", "ModLFO2Vol", "Unused1", "ChorusSend", "ReverbSend", "Pan", "Unused2", "Unused3", "Unused4", "ModLFODelay", "ModLFOFreq", "VibLFODelay", "VibLFOFreq", "ModEnvDelay", "ModEnvAttack", "ModEnvHold", "ModEnvDecay", "ModEnvSustain", "ModEnvRelease", "Key2ModEnvHold", "Key2ModEnvDecay", "VolEnvDelay", "VolEnvAttack", "VolEnvHold", "VolEnvDecay", "VolEnvSustain", "VolEnvRelease", "Key2VolEnvHold", "Key2VolEnvDecay", "Instrument", "Reserved1", "KeyRange", "VelRange", "StartLoopAddrCoarseOfs", "Keynum", "Velocity", "Attenuation", "Reserved2", "EndLoopAddrCoarseOfs", "CoarseTune", "FineTune", "SampleId", "SampleModes", "Reserved3", "ScaleTune", "ExclusiveClass", "OverrideRootKey", "Dummy"];

let rctx;
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
		chart(curve, document.querySelector("canvas"));
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

		const sr = e.target.getAttribute('sr');
		const startloop = e.target.getAttribute("startloop");
		const endloop = e.target.getAttribute("endloop")

		const file = e.target.getAttribute('file');
		const range = e.target.getAttribute('range');
		const attrs = e.target.parentElement.parentElement.querySelector(".attlist").getAttribute("attrs").split(",");
		const zone = new Proxy(attrs, {
			get: (target, attr) => genstrs.indexOf(attr) > -1 ? target[genstrs.indexOf(attr)] : null
		})
		console.log(zone.Attenuation)
		fetch(file, {
			headers: {
				Range: range
			}
		}).then(res => res.arrayBuffer()).then(ab => {

			const s16tof32 = (i16) => i16 / 0xffff;
			const ctx = new AudioContext({sampleRate: sr});
			const s16s = new Int16Array(ab)
			const audb = ctx.createBuffer(2, s16s.length, 48000);
			const buffer = audb.getChannelData(0);
			for (let i = 0;i < audb.length;i++) {
				buffer[i] = s16tof32(s16s[i]);
			}
			const abs = new AudioBufferSourceNode(ctx, {buffer: audb, loopStart: parseInt(startloop), loopEnd: parseInt(endloop)});
			abs.connect(ctx.destination);
			abs.start();
		}).catch(e => {
			console.error(e);
		});

	}
}