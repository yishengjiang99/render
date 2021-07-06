"use strict";
exports.__esModule = true;
exports.bindMidiAccess = void 0;
function bindMidiAccess(procPort, noteOn, noteOff, stdout, stderr) {
    // @ts-ignore
    return navigator.requestMIDIAccess().then(function (midiAccess) {
        stdout("midi access grant");
        var midiInputs = Array.from(midiAccess.inputs.values());
        for (var _i = 0, midiInputs_1 = midiInputs; _i < midiInputs_1.length; _i++) {
            var input = midiInputs_1[_i];
            // @ts-ignore
            input.onmidimessage = function (_a) {
                var data = _a.data, timestamp = _a.timestamp;
                //procPort.postMessage({ midi: data, timestamp });
                var channel = data[0] & 0x7f;
                var cmd = data[0] & 0x80;
                var note = data[1];
                var velocity = data.length > 2 ? data[2] : 0;
                switch (cmd) {
                    case 0x90:
                        noteOn(note, channel, velocity);
                        break;
                    case 0x80:
                        if (velocity == 0) {
                            noteOff(note, channel, 0);
                        }
                        else {
                            noteOn(note, channel, velocity);
                        }
                        break;
                    case 0x1a:
                        break;
                }
            };
        }
        midiAccess.onChange = function () { return stderr("midi access evoked"); };
        return midiInputs;
    }, function () {
        stderr("access not granted");
    });
}
exports.bindMidiAccess = bindMidiAccess;
