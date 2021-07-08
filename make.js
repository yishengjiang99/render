"use strict";
const fs = require("fs");
const execSync = require("child_process").execSync;

execSync(
  "emcc --no-entry -s EXPORTED_FUNCTIONS=['_upsample_wave_table'] sample.c -o wasmbuild/sample2.wasm"
);
fs.writeFileSync(
  `wasmbuild/sample.js`,
  `// prettier-ignore
  export const wasmBinary = new Uint8Array([
    ${fs.readFileSync("wasmbuild/sample2.wasm").join(",")}
  ]);
  

  
  `
);
