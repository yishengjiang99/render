#include <fluidsynth.h>
int main()
{

	fluid_settings_t *settings;
	settings = new_fluid_settings();
	fluid_synth_t *t = new_fluid_synth(settings);
	fluid_audio_driver_t *a = new_fluid_audio_driver(settings, t);
	fluid_sfloader_t *loader = new_fluid_defsfloader(settings);
	fluid_synth_sfload(t, "file.sf2", 1);
	/* Play a note */
	fluid_synth_noteon(t, 0, 60, 100);

	printf("Press \"Enter\" to stop: ");
	fgetc(stdin);
	printf("done\n");
}