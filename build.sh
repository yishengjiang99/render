cmake --build build
rm *.o && make
for i in $(seq 0 127); do
./fft-samples.o $i 0;
done
for i in $(seq 0 8); do
./fft-samples.o $i 128;
done