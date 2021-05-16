var Fs = require("fs");
var spawn = require("child_process").spawn;
require("http")
    .createServer(function (req, res) {
    if (req.url == "/song.wav") {
        res.write(WAVheader(48000, 2));
        Fs.createReadStream("song.fifo").pipe(res);
        process.nextTick(function () {
            spawn("./midi", ["song.mid"]);
        });
    }
    if (req.url == "/song.mp3") {
        var render = spawn("./midi");
        var ffm2mp3 = spawn("ffmpeg", "-f f32le -ac 2 -i song.fifo -f mp3 -o -");
        require("child_process").pipeline(render, ffm2mp3).pipe(res); // pipe(spawn());
    }
    else {
        res.end(/* html */ "\n\t\t\t<!DOCTYPE html>\n\t<html>\n\t\t<body>\n\t\t<audio controls src='song.wav'></audio>\n\t\t</body>\n\t\t</html>");
    }
})
    .listen(3000);
function WAVheader(n, channel) {
    var buffer = new Uint8Array(44);
    var view = new DataView(buffer.buffer);
    function writeString(view, offset, string) {
        for (var i = 0; i < string.length; i++) {
            view.setUint8(offset + i, string.charCodeAt(i));
        }
    }
    /* RIFF identifier */
    writeString(view, 0, "RIFF");
    /* RIFF chunk length */
    view.setUint32(4, 36 + n * 4, true);
    /* RIFF type */
    writeString(view, 8, "WAVE");
    /* format chunk identifier */
    writeString(view, 12, "fmt ");
    /* format chunk length */
    view.setUint32(16, 32, true);
    /* sample format (raw) */
    view.setUint16(20, 0x0003, true);
    /* channel count */
    view.setUint16(22, 2, true);
    /* sample rate */
    view.setUint32(24, 48000, true);
    /* byte rate (sample rate * block align) */
    view.setUint32(28, 48000 * 8, true);
    /* block align (channel count * bytes per sample) */
    view.setUint16(32, channel * 8, true);
    /* bits per sample */
    view.setUint16(34, 32, true);
    /* data chunk identifier */
    writeString(view, 36, "data");
    /* data chunk length */
    view.setUint32(40, n * 8, true);
    return buffer;
}
