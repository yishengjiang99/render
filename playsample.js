import {mkcanvas,resetCanvas,chart,HEIGHT,WIDTH} from './node_modules/chart/chart.js';
import { fftmod } from "./FFT.js";
const n=8 ,N=1<<n;
const {
  FFT,iFFT,
  complexRef,
  complex,
  heap,
  reset,
  getFloatFrequencyData,
  inputPCM,getWaveForm,
} = await fftmod(n);
document.body.style='background-color:black;color:white;'
let pt, ctx, sharedBuffer;
const pcmCache = new WeakMap();
const cans=[1,2,3,4,5,6,7,8].map(a=>mkcanvas(document.querySelector("aside")));
let realaud;
const genstraa=genstr();
async function run_sf2_smpl(e) {
	if (e.target.classList.contains('pcm'))
	{
    e.preventDefault();
    document.querySelector("main").style.width='10vw';
    const attbag = e.target.parentElement.querySelector(".attlist");
    if (attbag == null) return;
    const [sr, startloop, pitch, endloop, file, range] =
      "sr,startloop,pitch,endloop,file,range"
        .split(",")
        .map((i, id) =>
          id < 4 ? parseInt(attbag.getAttribute(i)) : attbag.getAttribute(i)
        );
    const attrs = attbag.getAttribute("zone").split(",");
    const zone = new Proxy(attrs, {
      get: (target, key) =>
      genstraa.indexOf(key) > -1
          ? parseInt(target[genstraa.indexOf(key)])
          : null,
    });
    const mid = parseInt(e.target.getAttribute("midi")) || zone.VelRange & 0x7f;

    const rangeHeaders = {
      headers: {
        Range: range,
      },
    };
    const cacheKey = rangeHeaders;
    if (!pcmCache.has(cacheKey)) {
      const pcm = await (await fetch(file, rangeHeaders)).arrayBuffer();
      pcmCache.set(cacheKey, pcm);
    }
    const ab = pcmCache.get(cacheKey);

    const s16tof32 = (i16) => i16 / 0xffff;

    const s16s = new Int16Array(ab);
    const ctx = new OfflineAudioContext(1, 48000 * 2.0, 48000);
    
    const audb = ctx.createBuffer(2, s16s.length, sr);
    const buffer = audb.getChannelData(0);
    for (let i = 0; i < audb.length; i++) {
      buffer[i] = s16tof32(s16s[i]);
    }
    realaud =  new AudioContext(ctx, { sampleRate: 48000 });

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
    const cent2sec = (cent) => Math.pow(2, cent / 1200);
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
		abs.connect(volumeEnveope)
      .connect(lpf)
      .connect(ctx.destination);
    abs.start();
    e.target.addEventListener("mouseup",e=>{
      volumeEnveope.gain.linearRampToValueAtTime(
        Math.pow(10, -zone.Attenuation / 200),
        Math.pow(2, zone.VolEnvRelease / 1200)
      );
    },{once:true});
    abs.stop(13);
    ctx.startRendering().then(ab=>{
      const abss=  new AudioBufferSourceNode(realaud,{
        buffer: ab
      });
      abss.connect(realaud.destination);
      abss.start();
      

    });

   
  }
}
window.onmousedown=run_sf2_smpl;



function updateFFT(fftr) {

  fftr.fftSize = 256;
  var bufferLength = fftr.frequencyBinCount;
  var dataArray = new Float32Array(bufferLength);
  const canvasCtx = mkcanvas();
  canvasCtx.clearRect(0, 0, WIDTH, HEIGHT);
  var drawVisual;

  function draw() {
    drawVisual = requestAnimationFrame(draw);

    fftr.getFloatFrequencyData(dataArray);

    canvasCtx.fillStyle = "rgb(0, 0, 0)";
    canvasCtx.fillRect(0, 0, WIDTH, HEIGHT);

    var barWidth = (WIDTH / bufferLength) * 2.5;
    var barHeight;
    var x = 0;

    for (var i = 0; i < bufferLength; i++) {
      barHeight = (dataArray[i] + 140) * 2;

      canvasCtx.fillStyle =
        "rgb(" + Math.floor(barHeight + 100) + ",50,50)";
      canvasCtx.fillRect(
        x,
        HEIGHT - barHeight / 2,
        barWidth,
        barHeight / 2
      );

      x += barWidth + 1;
    }
  }

  draw();
}


const rendctx=new Array(16).fill(0);
function setProgram(pid, chan){
  rendctx[chan]=pid;
  document.querySelector("")
  }
function keyOn(pid,mid,vel){

}

function genstr(){ return ["StartAddrOfs", "EndAddrOfs", "StartLoopAddrOfs", "EndLoopAddrOfs", "StartAddrCoarseOfs", "ModLFO2Pitch", "VibLFO2Pitch", "ModEnv2Pitch", "FilterFc", "FilterQ", "ModLFO2FilterFc", "ModEnv2FilterFc", "EndAddrCoarseOfs", "ModLFO2Vol", "Unused1", "ChorusSend", "ReverbSend", "Pan", "Unused2", "Unused3", "Unused4", "ModLFODelay", "ModLFOFreq", "VibLFODelay", "VibLFOFreq", "ModEnvDelay", "ModEnvAttack", "ModEnvHold", "ModEnvDecay", "ModEnvSustain", "ModEnvRelease", "Key2ModEnvHold", "Key2ModEnvDecay", "VolEnvDelay", "VolEnvAttack", "VolEnvHold", "VolEnvDecay", "VolEnvSustain", "VolEnvRelease", "Key2VolEnvHold", "Key2VolEnvDecay", "Instrument", "Reserved1", "KeyRange", "VelRange", "StartLoopAddrCoarseOfs", "Keynum", "Velocity", "Attenuation", "Reserved2", "EndLoopAddrCoarseOfs", "CoarseTune", "FineTune", "SampleId", "SampleModes", "Reserved3", "ScaleTune", "ExclusiveClass", "OverrideRootKey", "Dummy"];}
function pt_code() {
	return URL.createObjectURL(new Blob([`class PtProc extends AudioWorkletProcessor {
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
				// for(let i=0;i<inputList[0][0].length; i++){
				// 	this.disk[this.wptr+=2]=inputList[0][0];
				// }
				// if(this.wptr>N*2) this.wptr=0;
			}

			return true;
		}
	}
	// @ts-ignore
	registerProcessor("pt", PtProc);`], { 'type': 'application/javascript' }));
}

async function playbins(e) {

	rctx = rctx || new AudioContext();
	const real = e.target.getAttribute("real").split(",");
	const imag = e.target.getAttribute("img").split(",");
	let ctx = new OfflineAudioContext({
		numberOfChannels: 1,
		length: N,
		sampleRate: N,
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