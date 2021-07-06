import { fftmod } from "./FFT.js";
const {
  FFT,
  iFFT,
  stblRef,
  getWaveForm,
  stbl,
  complexRef,
  complex,
  instance,
  heap,
  module,
  getFloatFrequencyData,
  inputPCM,
} = await fftmod(5);
const {
  FFT,
  iFFT,
  stblRef,
  getWaveForm,
  stbl,
  complexRef,
  complex,
  bit_reverse,
  instance,
  heap,
  module,
  getFloatFrequencyData,
  inputPCM,
} = await fftmod(8);
complex.set(new Float64Array(32).fill(0));
complex[5] = 11.1;
complex[11] = 11.1;

bit_reverse(complexRef, 5);
const r = getWaveForm();

const hanning = [
  0, 0.01023503, 0.04052109, 0.08961828, 0.15551654, 0.23551799, 0.32634737,
  0.42428611, 0.52532458, 0.62532627, 0.72019708, 0.80605299, 0.87937906,
  0.93717331, 0.97706963, 0.99743466, 0.99743466, 0.97706963, 0.93717331,
  0.87937906, 0.80605299, 0.72019708, 0.62532627, 0.52532458, 0.42428611,
  0.32634737, 0.23551799, 0.15551654, 0.08961828, 0.04052109, 0.01023503, 0,
];
const c = mkcanvas();
k = -4;
function looop() {
  chart(
    c,
    hanning.map((v, i) => r[0][i + k] * v)
  );
  k++;
  requestAnimationFrame(looop);
}
