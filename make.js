"use strict";
const fs = require("fs");
const execSync = require("child_process").execSync;

execSync("npx wa compile -g 3 -s 120 sample.c -o wasmbuild/sample.wasm");
fs.writeFileSync(
  `wasmbuild/sample.js`,
  `// prettier-ignore
  export const wasmBinary = new Uint8Array([
    ${fs.readFileSync("wasmbuild/sample.wasm").join(",")}
  ]);
  const mem = new WebAssembly.Memory({
    initial: 1,
    maximum: 1,
  });

  const module = new WebAssembly.Module(wasmBinary);

  const instance = new WebAssembly.Instance(module, {
    env: {
      memory: mem,   
    },
  });
  export {
    init: instance.exports.init,
    upsample_wave_table: instance.exports.upsample_wave_table,
    heap; new Uint8Array(mem.buffer)
  }
  
  `
);
