gcc -c -fPIC -DPIC ../src/linux_sluice.c -o sluice.o
ld -shared -o sluice.so sluice.o
