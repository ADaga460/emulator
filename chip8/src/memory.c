// memory.c

/*
Concepts:
    Implementation of memory.h functions for CHIP-8 emulator.
    Memory initialization, ROM loading, reading, and writing.
    What is ROM loading? It is reading a program file into the emulator's memory so it can be executed.
    How does reading and writing work? By accessing specific memory addresses in the data array.
    How is this initialized? By setting all memory bytes to zero.

    What does memory_load_rom function do? It loads a ROM file into the memory starting at address 0x200.
*/
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void memory_init(Memory *m) {
    memset(m->data, 0, MEMORY_SIZE);
}

void memory_load_rom(Memory *m, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("fopen ROM");
        exit(1);
    }

    // Seek to end to get size
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (sz <= 0) {
        fclose(f);
        fprintf(stderr, "Empty ROM or read error\n");
        exit(1);
    }

    if (ROM_START + sz > MEMORY_SIZE) {
        fclose(f);
        fprintf(stderr, "ROM too large to fit memory\n");
        exit(1);
    }

    size_t read = fread(&m->data[ROM_START], 1, sz, f);
    if ((long)read != sz) {
        fclose(f);
        fprintf(stderr, "Short read\n");
        exit(1);
    }

    fclose(f);
}

uint8_t memory_read(Memory *m, uint16_t address) {
    if (address >= MEMORY_SIZE) return 0;
    return m->data[address];
}

void memory_write(Memory *m, uint16_t address, uint8_t value) {
    if (address < MEMORY_SIZE) m->data[address] = value;
}
