#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
/*
Concepts:
    Implementation of memory.h functions for CHIP-8 emulator.
    Memory initialization, ROM loading, reading, and writing.
    What is ROM loading? It is reading a program file into the emulator's memory so it can be executed.
    How does reading and writing work? By accessing specific memory addresses in the data array.
    How is this initialized? By setting all memory bytes to zero.
*/
void memory_init(Memory *m) {
    for (int i = 0; i < MEMORY_SIZE; i++) m->data[i] = 0;
}

void memory_load_rom(Memory *m, const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) { perror("ROM"); exit(1); }

    size_t offset = 0x200;
    fread(&m->data[offset], 1, MEMORY_SIZE - offset, f);
    fclose(f);
}

uint8_t memory_read(Memory *m, uint16_t address) {
    if (address >= MEMORY_SIZE) return 0;
    return m->data[address];
}

void memory_write(Memory *m, uint16_t address, uint8_t value) {
    if (address < MEMORY_SIZE) m->data[address] = value;
}
