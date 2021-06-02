fft-samples.o: fft.o wavetable_oscillator.o sf2.o
	gcc fft.o wavetable_oscillator.o sf2.o fft-samples.c -o fft-samples.o

fft.o :
	gcc -c libs/fft.c -o fft.o

wavetable_oscillator.o :
	gcc -c libs/wavetable_oscillator.c -o wavetable_oscillator.o

sf2.o :
	gcc -c sf2.c -o sf2.o

runtime.o :
	gcc -c runtime.c -o runtime.o


