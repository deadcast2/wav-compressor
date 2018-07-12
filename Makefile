CC = x86_64-w64-mingw32-gcc
CFLAGS = -std=c99 -O2
LDFLAGS = -mconsole
LDLIBS = -lkernel32

wav-compressor.exe: main.c
	$(CC) $(LDFLAGS) $(CFLAGS) fastlz.c main.c -o $@ $(LDLIBS)

clean:
	rm -f *.exe *.o
