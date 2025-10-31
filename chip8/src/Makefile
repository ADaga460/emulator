CC=gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
LDFLAGS = -L/opt/homebrew/lib -lSDL2
OBJ=main.o chip8.o memory.o cpu.o display_sdl.o sound.o

chip8: $(OBJ)
	$(CC) $(CFLAGS) -o chip8 $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f chip8 $(OBJ)

.PHONY: clean