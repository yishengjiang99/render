all: runtime.o midi.o html.o

# fft-samples.o: sf2.o fft.o wavetable_oscillator.o runtime.o 
# 	gcc sf2.o fft.o wavetable_oscillator.o runtime.o fft-samples.c -o fft-samples.o

midi.o: runtime.o sf2.o
	gcc libs/biquad.c runtime.o sf2.o midi.c -o midi.o
	
fft.o :
	gcc -c libs/fft.c -o fft.o

wavetable_oscillator.o :
	gcc -c libs/wavetable_oscillator.c -o wavetable_oscillator.o

sf2.o :
	gcc -c sf2.c -o sf2.o

runtime.o :
	gcc -c runtime.c -o runtime.o


html.o: sf2.o
	gcc html.c sf2.o -o html.o
	
clean:
	rm *.o