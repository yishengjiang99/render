const c=new AudioContext({sampleRate:96000}); 
c.audioWorklet.addModule(URL.createObjectURL(new Blob(
	[ /* javascript */ `class t extends AudioWorkletProcessor
	process(_, outputs){
	debugger;
	}
	}
	registerProcessor("test",t);`
`],type:"application/javascript")));