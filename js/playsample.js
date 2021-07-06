"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
exports.__esModule = true;
var chart_js_1 = require("../chart/chart.js");
var fft_js_1 = require("../fft.js");
var FFTSize = 1024;
var fft = fft_js_1.fftmod(5);
var bzero = function (arr) { return arr.forEach(function (v) { return (v = 0); }); };
var chunk = 2 * FFTSize * Float64Array.BYTES_PER_ELEMENT;
var sharedBuffer = new SharedArrayBuffer(6 * chunk);
var sharedBufferPool = [new Float64Array(sharedBuffer, chunk)];
var workerPool = [];
var fftcanvasContainer = document.getElementById("fft_canvas_container");
fftcanvasContainer.setAttribute("height", "" + chart_js_1.HEIGHT);
fftcanvasContainer.setAttribute("width", "" + chart_js_1.WIDTH);
var FFTSpectrumBarChart_get_singleton = function () {
    if (fftcanvasContainer.querySelectorAll("canvas").length > 0)
        fftcanvasContainer.innerHTML = "";
    var elem = document.createElement("canvas");
    fftcanvasContainer.appendChild(elem);
    var offline = elem.transferControlToOffscreen();
    return offline;
};
/**
 * fetch with cache layer a section of bit16 pcm at within a byte range
 * and convert to f32s
 * @url hardcodeed to 'file.sf2' for now!
 * @range 'bytes=%u-%u',startbyte, endbyte
 *
 * @returns float32Array
 */
var pcmCache = new Float32Array(1014 * 1014);
function resolvePCM(url, range) {
    return __awaiter(this, void 0, void 0, function () {
        var rangeHeaders, pcm, _a, fl32s, i;
        return __generator(this, function (_b) {
            switch (_b.label) {
                case 0:
                    rangeHeaders = {
                        headers: {
                            Range: range
                        }
                    };
                    _a = Int16Array.bind;
                    return [4 /*yield*/, fetch(url, rangeHeaders)];
                case 1: return [4 /*yield*/, (_b.sent()).arrayBuffer()];
                case 2:
                    pcm = new (_a.apply(Int16Array, [void 0, _b.sent()]))();
                    fl32s = new Float32Array(pcm.length);
                    for (i = 0; i < pcm.length; i++) {
                        fl32s[i] = pcm[i] / 0xffff;
                    }
                    return [2 /*return*/, fl32s];
            }
        });
    });
}
/**
 * @param AudioContext
 */
var realctx;
/**
 * proxys comma-separated str of attributes into
 * dot-accessing objects to make beter autocompletes in vscode
 * @param attrs csv strings
 * @returns Proxy<string,number>
 */
function newSFZone(attrs) {
    var attributeKeys = genstr();
    var attributeValues = attrs.split(",").map(function (s) { return parseInt(s); });
    return new Proxy(attributeValues, {
        get: function (target, key) {
            var idx = attributeKeys.indexOf(key);
            return idx > -1 ? target[idx] : null;
        }
    });
}
/**
 *
 * @param {HTMLElement|XMLElement} attbag anchor link next ot sample link
 * with attributes describing font. to take advantage of DOM Tree indexing with chrome hogs up 2G of ram
 *
 * @return {<string, string|number>} sr, startloop, pitch, endloop, file, range
 */
var parseSampleInfo = function (attbag) {
    var sr = parseInt(attbag.getAttribute("sr")), startloop = parseInt(attbag.getAttribute("startloop")), pitch = parseFloat(attbag.getAttribute("pitch")).toFixed(1), endloop = parseInt(attbag.getAttribute("endloop")), file = attbag.getAttribute("file"), range = attbag.getAttribute("range");
    console.assert(!isNaN(sr)); // === false);
    return { sr: sr, startloop: startloop, pitch: pitch, endloop: endloop, file: file, range: range };
};
var rightPanel = document.querySelector("#details");
document.querySelector("main").style.width = "10vw";
var canvas = chart_js_1.mkcanvas(rightPanel);
var canvas2 = FFTSpectrumBarChart_get_singleton();
var cent2sec = function (cent) { return Math.pow(2, cent / 1200); };
var attackEvents = [
    "touchstart",
    "click",
    "focus",
    "keydown",
    "mousedown",
    "wheel",
];
var releaseEventsList = [
    "touchend",
    "mouseup",
    "unfocus",
    "keyup",
    "mouseup",
    "wheel",
];
var touchHandleSemaphored = 0;
attackEvents.map(function (eventName, idx) { });
window.onmousedown = function (e) { return __awaiter(void 0, void 0, void 0, function () {
    var btn, onRelease, attbag, info, zone, smplData, midi;
    return __generator(this, function (_a) {
        switch (_a.label) {
            case 0:
                btn = e.target;
                onRelease = new Promise(function (r) {
                    return window.addEventListener("mouseup", r, { once: true });
                });
                if (!e.target.classList.contains("pcm")) return [3 /*break*/, 2];
                attbag = btn.parentElement.querySelector(".attlist");
                console.assert(attbag != null);
                info = parseSampleInfo(attbag);
                zone = newSFZone(attbag.getAttribute("zone"));
                return [4 /*yield*/, resolvePCM(info.file, info.range)];
            case 1:
                smplData = _a.sent();
                midi = parseInt(btn.getAttribute("midi") || "60");
                run_sf2_smpl(parseSampleInfo(attbag), zone, smplData, midi).then(function (triggerAdsrRelease) {
                    onRelease.then(triggerAdsrRelease);
                });
                if (!realctx)
                    realctx = new AudioContext();
                if (realctx.state == "suspended")
                    realctx.resume().then(function (ab) { });
                _a.label = 2;
            case 2: return [2 /*return*/];
        }
    });
}); };
function run_sf2_smpl(sampleInfo, zone, smplData, midi) {
    return __awaiter(this, void 0, void 0, function () {
        function renderLoop() {
            chart_js_1.chart(canvas, ab.getChannelData(0).slice(readoffset, readoffset + 1024));
            readoffset += 1024;
            if (readoffset > flnum)
                return;
            else
                requestAnimationFrame(renderLoop);
        }
        var sr, startloop, pitch, endloop, file, range, ctx, audb, abs, lpf, modEnvelope, volumeEnveope, releaseTime, rendJoin, ab, flnum, readoffset, abss;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    sr = sampleInfo.sr, startloop = sampleInfo.startloop, pitch = sampleInfo.pitch, endloop = sampleInfo.endloop, file = sampleInfo.file, range = sampleInfo.range;
                    ctx = new OfflineAudioContext(1, sr * 2.0, sr);
                    audb = ctx.createBuffer(2, smplData.length, sr);
                    audb.getChannelData(0).set(smplData);
                    abs = new AudioBufferSourceNode(ctx, {
                        buffer: audb,
                        playbackRate: 1,
                        loop: true,
                        loopStart: startloop / sr,
                        loopEnd: endloop / sr
                    });
                    lpf = new BiquadFilterNode(ctx, {
                        frequency: Math.min(Math.pow(2, zone.FilterFc / 1200) * 8.176, ctx.sampleRate / 2),
                        Q: zone.FilterQ / 10,
                        type: "lowpass"
                    });
                    modEnvelope = new GainNode(ctx, { gain: 0 });
                    modEnvelope.connect(lpf.frequency);
                    if (zone.ModEnvAttack > -12000) {
                        modEnvelope.gain.linearRampToValueAtTime(1, cent2sec(zone.ModEnvAttack)); //Math.pow(2, (zone.ModEnvAttack) / 1200));
                    }
                    else {
                        modEnvelope.gain.value = 1.0;
                    }
                    modEnvelope.gain.setTargetAtTime(1 - zone.ModEnvSustain / 1000, cent2sec(zone.ModEnvDecay), 0.4);
                    volumeEnveope = new GainNode(ctx, { gain: 0 });
                    volumeEnveope.gain.linearRampToValueAtTime(Math.pow(10, -zone.Attenuation / 200), Math.pow(2, zone.VolEnvAttack / 1200));
                    volumeEnveope.gain.setTargetAtTime(1 - zone.VolEnvSustain / 1000, Math.pow(2, zone.VolEnvDecay / 1200), 0.4);
                    releaseTime = Math.pow(2, zone.VolEnvRelease / 1200);
                    abs.connect(volumeEnveope).connect(lpf).connect(ctx.destination);
                    abs.start();
                    rendJoin = ctx.startRendering();
                    return [4 /*yield*/, rendJoin];
                case 1:
                    ab = _a.sent();
                    flnum = ab.getChannelData(0).length;
                    readoffset = 0;
                    renderLoop();
                    abss = new AudioBufferSourceNode(realctx, {
                        buffer: ab
                    });
                    abss.connect(realctx.destination);
                    abss.start();
                    abss.onended = function () { };
                    return [2 /*return*/, function envrelease() {
                            volumeEnveope.gain.linearRampToValueAtTime(0, releaseTime);
                        }];
            }
        });
    });
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
    return URL.createObjectURL(new Blob([
        "\n        class PtProc extends AudioWorkletProcessor {\n\t\tconstructor(options) {\n\t\t\tsuper(options);\n\t\t\t// this.woffset = woffset;\n\t\t\t// this.timestampOffset = timestampOffset;\n\t\t\t\tthis.disk = new Float64Array(options.processorOptions.sharedBuffer);\n\t\t\t\tthis.wptr=0;\n\t\t}\n\t\tprocess(inputList, outputList) {\n\t\t\t//@ts-ignore\n\t\t\tif(inputList[0] && inputList[0][0]){\n\t\t\t\toutputList[0][0].set(inputList[0][0]);\n\t\t\t\tfor(let i=0;i<inputList[0][0].length; i++){\n\t\t\t\t\tthis.disk[this.wptr+=2]=inputList[0][0];\n\t\t\t\t}\n\t\t\t\tif(this.wptr>N*2) this.wptr=0;\n\t\t\t}\n\n\t\t\treturn true;\n\t\t}\n\t}\n\t// @ts-ignore\n\tregisterProcessor(\"pt\", PtProc);",
    ], { type: "application/javascript" }));
}
