
//header got
const respone = await fetch("https://tonejs.github.io/audio/salamander/Ds2.mp3");



.then(res => res.arrayBuffer()).then(ab => (new AudioContext()).decodeAudioData(ab)).then(audb => new AudioBufferSourceNode(audb))