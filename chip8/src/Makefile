CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -O2
OBJ=main.o chip8.o memory.o cpu.o

chip8: $(OBJ)
	$(CC) $(CFLAGS) -o chip8 $(OBJ)

clean:
	rm -f chip8 $(OBJ)