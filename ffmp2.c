#include <stdio.h>
#define compression "compand=.3|.3:1|1:-90/-60|-60/-40|-40/-30|-20/-20:6:0:-90:0.2"
#define audio_filter_path(filtercmd) "[0:a]##filtercmd##[audio]"
#define compression "[0:a][audio]"
#define map_aud_filter "-map 0:v -map [audio] -codec:v copy output"
#define stdfmt "-f f32le -ac 2 -r 32"
#define quiet "-hide-banner -loglevel panic"
#define sink_to_ffplay "|ffplay -i pipe:0"
#define filter_then_play "ffmpeg "
FILE *ffp(int ac, int ar, char *filters)
{
	char cmd[1024];
	sprintf(cmd, "ffmpeg %s %s -i pipe:0 -f f32le -ac %d -ar %d", ac, ar);
	sprintf(cmd, "%s -filter-complex %s %s", cmd, audio_filter_path(filtercmd), map_aud_filter);
	FILE *ffplay = popen(cmd, "w");
	if (!ffplay)
		perror("cmd fail");
	return ffplay;
}
//*-filter_complex \
"[0:a]compand=.3|.3:1|1:-90/-60|-60/-40|-40/-30|-20/-20:6:0:-90:0.2[audio]" \
-map 0:v -map "[audio]" -codec:v copy output*/
// FILE *loudnorm(int ac, int ar)
// {
//   char cmd[1024];
//   sprintf(cmd, "ff", ac, ar);
//   FILE *ffplay = popen(cmd, "w");
//   if (!ffplay)
//     perror("cmd fail");
//   return ffplay;
// }

FILE *wavepic(char *png_name)
{
	char cmd[1024];
	sprintf(cmd, "ffmpeg -i pipe:0 -filter_complex 'showwavespic=s=640x120' -frames:v 1 %s", png_name); // ac, ar);
	FILE *ffwavepic = popen(cmd, "w");
	if (!ffwavepic)
		perror("cmd fail");
	return ffwavepic;
}

FILE *formatpcm(char *format, char *filename)
{
	char cmd[1024];
	sprintf(cmd, "ffmpeg -y -f f32le -i pipe:0 -ac 2 -ar 48000 -f %s %s", format, filename); //png_name); // ac, ar);
	FILE *pipel = popen(cmd, "w");
	if (!pipel)
		perror("cmd fail");
	return pipel;
}
//popen("ffmpeg -y -f f32le -i pipe:0 -ac 2 -ar 48000 -f WAV o.wav", "w")