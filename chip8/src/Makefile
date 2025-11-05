CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2 -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
LDFLAGS = -L/opt/homebrew/lib -lSDL2

OBJS = main.o chip8.o memory.o cpu.o display_sdl.o sound.o

chip8: $(OBJS)
	$(CC) $(OBJS) -o chip8 $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f chip8 $(OBJS)
