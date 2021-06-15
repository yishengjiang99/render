all: fft-samples.o html.o

deps : runtime.c sf2.c libs/fft.c libs/wavetable_oscillator.c  libs/biquad.c
	gcc -c runtime.c sf2.c libs/fft.c libs/wavetable_oscillator.c libs/biquad.c

fft-samples.o: 
	gcc fft-samples.c -o fft-samples.o

html.o:
	gcc html.c -o html.o
	
clean:
	rm *.o