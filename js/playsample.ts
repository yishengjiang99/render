import { mkcanvas, resetCanvas, chart, HEIGHT, WIDTH } from "../chart/chart.js";
import { fftmod } from "../fft/FFT.js";
type PowerOfTwo = 1024 | 32 | 64 | 128 | 4096; //eye-balling the math here
const FFTSize: PowerOfTwo = 1024;
const fft = fftmod(1 << FFTSize);
const bzero = (arr: Float64Array) => arr.forEach((v) => (v = 0));
/**
 * fetch with cache layer a section of bit16 pcm at within a byte range
 * and convert to f32s
 * @url hardcodeed to 'file.sf2' for now!
 * @range 'bytes=%u-%u',startbyte, endbyte
 *
 * @returns float32Array
 */
async function resolvePCM(url: string, range: string) {
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
let realctx: AudioContext;
/**
 * proxys comma-separated str of attributes into
 * dot-accessing objects to make beter autocompletes in vscode
 * @param attrs csv strings
 * @returns Proxy<string,number>
 */
function newSFZone(attrs: string) {
  const attributeKeys = genstr();
  const attributeValues = attrs.split(",").map((s) => parseInt(s));
  return new Proxy(attributeValues, {
    get: (target, key: string) => {
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
const parseSampleInfo = (attbag: HTMLElement) => {
  const sr = parseInt(attbag.getAttribute("sr")!),
    startloop = parseInt(attbag.getAttribute("startloop")!),
    pitch = parseFloat(attbag.getAttribute("pitch")!).toFixed(1),
    endloop = parseInt(attbag.getAttribute("endloop")!),
    file = attbag.getAttribute("file"),
    range = attbag.getAttribute("range");
  console.assert(!isNaN(sr)); // === false);
  return { sr, startloop, pitch, endloop, file, range };
};
const rightPanel: HTMLElement = document.querySelector("#details")!;
document.querySelector("main")!.style.width = "10vw";
const canvas = mkcanvas(rightPanel);
const canvas2 = mkcanvas(rightPanel);

const cent2sec = (cent: number) => Math.pow(2, cent / 1200);

window.onmousedown = async (e) => {
  const btn = e.target as HTMLElement;
  /* 
    listen of release event once,may happen before rendering complete
    want to always release volume envelope
    */
  const onRelease = new Promise((r) =>
    window.addEventListener("mouseup", r, { once: true })
  );
  if ((e.target as HTMLElement).classList.contains("pcm")) {
    const attbag: HTMLElement = btn.parentElement!.querySelector(".attlist")!;
    console.assert(attbag != null);
    const info = parseSampleInfo(attbag);
    const zone = newSFZone(attbag.getAttribute("zone")!);
    const smplData = await resolvePCM(
      info.file as string,
      info.range as string
    );
    const midi = parseInt(btn.getAttribute("midi") || "60");
    run_sf2_smpl(parseSampleInfo(attbag), zone, smplData, midi).then(
      (triggerAdsrRelease) => {
        onRelease.then(triggerAdsrRelease);
      }
    );
    if (!realctx) realctx = new AudioContext();
    if (realctx.state == "suspended") realctx.resume().then((ab) => {});
  }
};
async function run_sf2_smpl(
  sampleInfo: any,
  zone: any,
  smplData: any,
  midi: any
) {
  const { sr, startloop, pitch, endloop, file, range } = sampleInfo;
  const ctx = new OfflineAudioContext(1, sr, sr);
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

  const rendJoin = ctx.startRendering();
  const ab = await rendJoin;

  const flnum = ab.getChannelData(0).length;
  let readoffset = 0;
  function renderLoop() {
    chart(canvas, ab.getChannelData(0).slice(readoffset, readoffset + 4096));
    readoffset += 4096;
    if (readoffset > flnum) return;
    else requestAnimationFrame(renderLoop);
  }

  renderLoop();

  const abss = new AudioBufferSourceNode(realctx, {
    buffer: ab,
    playbackRate: (midi * 100) / pitch,
  });
  abss.connect(realctx.destination);
  abss.start();
  abss.onended = () => {};
  return function envrelease() {
    volumeEnveope.gain.linearRampToValueAtTime(0, releaseTime);
  };
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
