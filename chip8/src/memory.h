/*
Concept
    CHIP-8 memory is 4 KB (4096 bytes).
    0x000–0x1FF = reserved for interpreter / fonts.
    Program ROMs load at 0x200.
    Emulated like a simple array of bytes.
    This shows how higher-level memory abstractions map to linear address spaces in hardware. Fun!
*/

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#define MEMORY_SIZE 4096
#define ROM_START 0x200

typedef struct {
    uint8_t data[MEMORY_SIZE];
} Memory;

void memory_init(Memory *m);
void memory_load_rom(Memory *m, const char *filename);
uint8_t memory_read(Memory *m, uint16_t address);
void memory_write(Memory *m, uint16_t address, uint8_t value);

#endif
