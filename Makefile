CC = x86_64-w64-mingw32-gcc
CFLAGS = -std=c99 -Os -Wall -Wextra -Werror
LDFLAGS = -s -nostdlib -mconsole
LDLIBS = -lkernel32

main.exe: main.c
	$(CC) $(LDFLAGS) $(CFLAGS) main.c -o $@ $(LDLIBS)

clean:
	rm -f *.exe *.o
