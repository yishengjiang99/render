cmake --build build
rm *.o && make
for i in $(seq 0 127); do
./fft-samples $i 0;
done
for i in $(seq 0 8); do
./fft-samples $i 128;
done