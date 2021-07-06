const resampleWorker = `
import { create, ConverterType } from "./libresam/libsamplerate.js";
addEventListener(
  "message",
  ({ data: { outputSampleRate, data, inputSampleRate } }) => {
    create(1, inputSampleRate, outputSampleRate, {
      wasmPath: "./libresam/libsamplerate.wasm",
    }).then((src) => {
      let resampledData = src.simple(data);

      postMessage(resampledData.buffer, "*", [resampledData.buffer]);
      src.destroy(); // clean up
    });
  }
);`;
const workerUrl = URL.createObjectURL(new Blob([resampleWorker], { type: "application/javascript" }));
const wk = new Worker(workerUrl, { type: "module" });
const ret = await new Promise((r) => {
    wk.onmessage = (e) => r(e);
    wk.postMessage({
        outputSampleRate: 100,
        inputSampleRate: 10,
        data: [1, 2, 3, 4, 3, 2, 1, 2, 3],
    });
});
export default ret;
