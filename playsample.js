const genstrs = ["Gen_StartAddrOfs", "Gen_EndAddrOfs", "Gen_StartLoopAddrOfs", "Gen_EndLoopAddrOfs", "Gen_StartAddrCoarseOfs", "Gen_ModLFO2Pitch", "Gen_VibLFO2Pitch", "Gen_ModEnv2Pitch", "Gen_FilterFc", "Gen_FilterQ", "Gen_ModLFO2FilterFc", "Gen_ModEnv2FilterFc", "Gen_EndAddrCoarseOfs", "Gen_ModLFO2Vol", "Gen_Unused1", "Gen_ChorusSend", "Gen_ReverbSend", "Gen_Pan", "Gen_Unused2", "Gen_Unused3", "Gen_Unused4", "Gen_ModLFODelay", "Gen_ModLFOFreq", "Gen_VibLFODelay", "Gen_VibLFOFreq", "Gen_ModEnvDelay", "Gen_ModEnvAttack", "Gen_ModEnvHold", "Gen_ModEnvDecay", "Gen_ModEnvSustain", "Gen_ModEnvRelease", "Gen_Key2ModEnvHold", "Gen_Key2ModEnvDecay", "Gen_VolEnvDelay", "Gen_VolEnvAttack", "Gen_VolEnvHold", "Gen_VolEnvDecay", "Gen_VolEnvSustain", "Gen_VolEnvRelease", "Gen_Key2VolEnvHold", "Gen_Key2VolEnvDecay", "Gen_Instrument", "Gen_Reserved1", "Gen_KeyRange", "Gen_VelRange", "Gen_StartLoopAddrCoarseOfs", "Gen_Keynum", "Gen_Velocity", "Gen_Attenuation", "Gen_Reserved2", "Gen_EndLoopAddrCoarseOfs", "Gen_CoarseTune", "Gen_FineTune", "Gen_SampleId", "Gen_SampleModes", "Gen_Reserved3", "Gen_ScaleTune", "Gen_ExclusiveClass", "Gen_OverrideRootKey", "Gen_Dummy"];
let rctx;
document.body.onclick = async (e) => {
	if (e.target.classList.contains("fftbins")) {
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