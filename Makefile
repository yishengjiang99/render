all: runtime.o midi.o html.o fft-samples.o

fft-samples.o: sf2.o fft.o wavetable_oscillator.o runtime.o 
	gcc sf2.o fft.o wavetable_oscillator.o runtime.o fft-samples.c -o fft-samples.o

midi.o:
	gcc midi.c -o midi.o
	
fft.o :
	gcc -c libs/fft.c -o fft.o

wavetable_oscillator.o :
	gcc -c libs/wavetable_oscillator.c -o wavetable_oscillator.o

sf2.o :
	gcc -c sf2.c -o sf2.o

runtime.o :
	gcc -c runtime.c -o runtime.o

html.o:
	gcc html.c -o html.o
	
clean:
	rm *.o