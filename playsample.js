let ctx;
document.body.onclick = (e) => {
	if (!e.target.classList.contains('pcm')) return;
	e.preventDefault();
	ctx = ctx || new AudioContext({sampleRate: 44100});
	const file = e.target.getAttribute('file');
	const range = e.target.getAttribute('range');
	fetch(file, {
		headers: {
			Range: range
		}
	}).then(res => res.arrayBuffer()).then(ab => {
		const s16tof32 = (i16) => (i16 > 0 ? i16 / 0x7fff : -1 - i16 / 0x8000);

		const s16s = new Int16Array(ab)
		const audb = ctx.createBuffer(2, s16s.length, 48000);
		const buffer = audb.getChannelData(0);
		for (let i = 0;i < audb.length;i++) {
			buffer[i] = s16tof32(s16s[i]);
		}
		const abs = new AudioBufferSourceNode(ctx, {buffer: audb});
		abs.connect(ctx.destination);
		abs.start();
	}).catch(e => {
		console.error(e);
	});
}