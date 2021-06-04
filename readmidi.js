const Midi = require("@tonejs/midi").Midi;
const {tracks, header} = new Midi(require("fs").readFileSync("./midifiles/billie-eilish-bad-guy-midi-by-carlo-prato-www-cprato-com-mid.mid").buffer);
console.log(header.ppq, header.tempos,);
tracks.forEach(t => {
	console.log(t.controlChanges)
})
//console.log(tracks.map(t => t.notes));