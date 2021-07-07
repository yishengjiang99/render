import { mkcanvas, chart } from "../chart/chart.js";
import { fftmod } from "../fft-64bits/FFT.js";
const FFTSize = 4096,
  n = 12;
let fftm;
fftmod(n).then((mod) => {
  fftm = mod;
});
const bzero = (arr) => arr.forEach((v) => (v = 0));
/**
 * fetch with cache layer a section of bit16 pcm at within a byte range
 * and convert to f32s
 * @url hardcodeed to 'file.sf2' for now!
 * @range 'bytes=%u-%u',startbyte, endbyte
 *
 * @returns float32Array
 */
async function resolvePCM(url, range) {
  const rangeHeaders = {
    headers: {
      Range: range,
    },
  };
  const pcm = new Int16Array(
    await (await fetch(url, rangeHeaders)).arrayBuffer()
  );
  const fl32s = new Float32Array(pcm.length);
  for (let i = 0; i < pcm.length; i++) {
    fl32s[i] = pcm[i] / 0xffff;
  }
  return fl32s;
}
/**
 * @param AudioContext
 */
let realctx;
/**
 * proxys comma-separated str of attributes into
 * dot-accessing objects to make beter autocompletes in vscode
 * @param attrs csv strings
 * @returns Proxy<string,number>
 */
function newSFZone(attrs) {
  const attributeKeys = genstr();
  const attributeValues = attrs.split(",").map((s) => parseInt(s));
  return new Proxy(attributeValues, {
    get: (target, key) => {
      const idx = attributeKeys.indexOf(key);
      return idx > -1 ? target[idx] : null;
    },
  });
}
/**
 *
 * @param {HTMLElement|XMLElement} attbag anchor link next ot sample link
 * with attributes describing font. to take advantage of DOM Tree indexing with chrome hogs up 2G of ram
 *
 * @return {<string, string|number>} sr, startloop, pitch, endloop, file, range
 */
const parseSampleInfo = (attbag) => {
  const sr = parseInt(attbag.getAttribute("sr")),
    startloop = parseInt(attbag.getAttribute("startloop")),
    pitch = parseFloat(attbag.getAttribute("pitch")).toFixed(1),
    endloop = parseInt(attbag.getAttribute("endloop")),
    file = attbag.getAttribute("file"),
    range = attbag.getAttribute("range");
  console.assert(!isNaN(sr)); // === false);
  return { sr, startloop, pitch, endloop, file, range };
};
const rightPanel = document.querySelector("#details");
document.querySelector("main").style.width = "10vw";
const canvas = mkcanvas(rightPanel);
const canvas1 = mkcanvas(rightPanel);

const canvas2 = mkcanvas(rightPanel);
const cent2sec = (cent) => Math.pow(2, cent / 1200);
const attackEvents = [
  "touchstart",
  "click",
  "focus",
  "keydown",
  "mousedown",
  "wheel",
];
const releaseEventsList = [
  "touchend",
  "mouseup",
  "unfocus",
  "keyup",
  "mouseup",
  "wheel",
];
let touchHandleSemaphored = 0;
attackEvents.map((eventName, idx) => {});
const hanning = new Float64Array(FFTSize);
for (let i = 0; i < FFTSize; i++) {
  hanning[i] = 0.5 - 0.5 * Math.cos((2 * Math.PI * n) / (FFTSize - 1));
}

window.onmousedown = async (e) => {
  const btn = e.target;
  /*
      listen of release event once,may happen before rendering complete
      want to always release volume envelope
      */
  if (!realctx) realctx = new AudioContext();

  if (e.target.classList.contains("pcm")) {
    const attbag = btn.parentElement.querySelector(".attlist");
    console.assert(attbag != null);
    const info = parseSampleInfo(attbag);
    const zone = newSFZone(attbag.getAttribute("zone"));
    const smplData = await resolvePCM(info.file, info.range);
    const midi = parseInt(btn.getAttribute("midi") || "60");
    const keyUpJoin = new Promise((r) =>
      window.addEventListener("mouseup", r, { once: true })
    );
    run_sf2_smpl(parseSampleInfo(attbag), zone, smplData, midi, keyUpJoin);
  }
};

async function run_sf2_smpl(sampleInfo, zone, smplData, midi, keyUpJoin) {
  const { sr, startloop, pitch, endloop, file, range } = sampleInfo;
  const ctx = new OfflineAudioContext(1, 4 * sr, sr);
  const audb = ctx.createBuffer(2, smplData.length, sr);
  audb.getChannelData(0).set(smplData);
  const abs = new AudioBufferSourceNode(ctx, {
    buffer: audb,
    playbackRate: 1,
    loop: true,
    loopStart: startloop / sr,
    loopEnd: endloop / sr,
  });
  let lpf = new BiquadFilterNode(ctx, {
    frequency: Math.min(
      Math.pow(2, zone.FilterFc / 1200) * 8.176,
      ctx.sampleRate / 2
    ),
    Q: zone.FilterQ / 10,
    type: "lowpass",
  });
  const modEnvelope = new GainNode(ctx, { gain: 0 });
  modEnvelope.connect(lpf.frequency);
  if (zone.ModEnvAttack > -12000) {
    modEnvelope.gain.linearRampToValueAtTime(1, cent2sec(zone.ModEnvAttack)); //Math.pow(2, (zone.ModEnvAttack) / 1200));
  } else {
    modEnvelope.gain.value = 1.0;
  }
  modEnvelope.gain.setTargetAtTime(
    1 - zone.ModEnvSustain / 1000,
    cent2sec(zone.ModEnvDecay),
    0.4
  );
  const volumeEnveope = new GainNode(ctx, { gain: 0 });
  volumeEnveope.gain.linearRampToValueAtTime(
    Math.pow(10, -zone.Attenuation / 200),
    Math.pow(2, zone.VolEnvAttack / 1200)
  );
  volumeEnveope.gain.setTargetAtTime(
    1 - zone.VolEnvSustain / 1000,
    Math.pow(2, zone.VolEnvDecay / 1200),
    0.4
  );
  const releaseTime = Math.pow(2, zone.VolEnvRelease / 1200);
  abs.connect(volumeEnveope).connect(lpf).connect(ctx.destination);
  abs.start();
  keyUpJoin.then(() =>
    volumeEnveope.gain.linearRampToValueAtTime(0, releaseTime)
  );

  const rendJoin = ctx.startRendering();
  const ab = await rendJoin;
  const flarr = ab.getChannelData(0);
  const flnum = flarr.length;
  let readoffset = 0;

  function renderLoop() {
    if (!fftm) return;

    const chunk = flarr.slice(readoffset, readoffset + FFTSize);
    const windowedInput = chunk.map((v, i) =>
      i < FFTSize ? v * hanning[i] : 0
    );

    fftm.inputPCM(windowedInput);
    const fftres = fftm.getFloatFrequencyData();

    chart(canvas, chunk);
    chart(canvas1, fftres[1].slice(0, FFTSize / 3));
    const nyquist_dogma = sr / 2;

    for (let i = 214; i < FFTSize; i++) {
      fftm.complex[i] = 0;
    }
    chart(canvas2, fftm.getWaveForm(0, FFTSize)[0]);

    readoffset += 1280;
    if (readoffset > flnum) return;
    else requestAnimationFrame(renderLoop);
  }
  requestAnimationFrame(renderLoop);
  const abss = new AudioBufferSourceNode(realctx, {
    buffer: ab,
    playbackRate: (midi * 100) / pitch,
  });
  abss.connect(realctx.destination);
  abss.start();
}
function genstr() {
  return [
    "StartAddrOfs",
    "EndAddrOfs",
    "StartLoopAddrOfs",
    "EndLoopAddrOfs",
    "StartAddrCoarseOfs",
    "ModLFO2Pitch",
    "VibLFO2Pitch",
    "ModEnv2Pitch",
    "FilterFc",
    "FilterQ",
    "ModLFO2FilterFc",
    "ModEnv2FilterFc",
    "EndAddrCoarseOfs",
    "ModLFO2Vol",
    "Unused1",
    "ChorusSend",
    "ReverbSend",
    "Pan",
    "Unused2",
    "Unused3",
    "Unused4",
    "ModLFODelay",
    "ModLFOFreq",
    "VibLFODelay",
    "VibLFOFreq",
    "ModEnvDelay",
    "ModEnvAttack",
    "ModEnvHold",
    "ModEnvDecay",
    "ModEnvSustain",
    "ModEnvRelease",
    "Key2ModEnvHold",
    "Key2ModEnvDecay",
    "VolEnvDelay",
    "VolEnvAttack",
    "VolEnvHold",
    "VolEnvDecay",
    "VolEnvSustain",
    "VolEnvRelease",
    "Key2VolEnvHold",
    "Key2VolEnvDecay",
    "Instrument",
    "Reserved1",
    "KeyRange",
    "VelRange",
    "StartLoopAddrCoarseOfs",
    "Keynum",
    "Velocity",
    "Attenuation",
    "Reserved2",
    "EndLoopAddrCoarseOfs",
    "CoarseTune",
    "FineTune",
    "SampleId",
    "SampleModes",
    "Reserved3",
    "ScaleTune",
    "ExclusiveClass",
    "OverrideRootKey",
    "Dummy",
  ];
}
function pt_code() {
  return URL.createObjectURL(
    new Blob(
      [
        `
        class PtProc extends AudioWorkletProcessor {
		constructor(options) {
			super(options);
			// this.woffset = woffset;
			// this.timestampOffset = timestampOffset;
				this.disk = new Float64Array(options.processorOptions.sharedBuffer);
				this.wptr=0;
		}
		process(inputList, outputList) {
			//@ts-ignore
			if(inputList[0] && inputList[0][0]){
				outputList[0][0].set(inputList[0][0]);
				for(let i=0;i<inputList[0][0].length; i++){
					this.disk[this.wptr+=2]=inputList[0][0];
				}
				if(this.wptr>N*2) this.wptr=0;
			}

			return true;
		}
	}
	// @ts-ignore
	registerProcessor("pt", PtProc);`,
      ],
      { type: "application/javascript" }
    )
  );
}