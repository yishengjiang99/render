cmake --build build
rm *.o && make && ./fft-samples.o
for i in $(seq 1 128); do
./fft-samples.o $i 0;
done