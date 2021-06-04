fft-samples.o: fft.o wavetable_oscillator.o runtime.o
	gcc fft.o wavetable_oscillator.o runtime.o fft-samples.c -o fft-samples.o

fft.o :
	gcc -c libs/fft.c -o fft.o

wavetable_oscillator.o :
	gcc -c libs/wavetable_oscillator.c -o wavetable_oscillator.o

sf2.o :
	gcc -c sf2.c -o sf2.o

runtime.o : sf2.o
	gcc sf2.o -c runtime.c -o runtime.o


html.o: sf2.o
	gcc html.c sf2.o -o html.o