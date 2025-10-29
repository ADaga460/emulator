#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include "memory.h"
#include "cpu.h"

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT)

/*
Concepts:
    The Chip8 structure encapsulates the entire state of a CHIP-8 emulator.
    It includes memory, CPU, display buffer, input keys, and a draw flag.
    This structure is essential for managing the emulator's state during execution.
*/
typedef struct {
    Memory memory;
    CPU cpu;
    uint8_t display[DISPLAY_SIZE]; // 0 or 1 per pixel
    uint8_t keys[16];              // 0 or 1 per key
    uint8_t draw_flag;             // set when display needs redraw
} Chip8;

void chip8_init(Chip8 *c);
void chip8_load_rom(Chip8 *c, const char *filename);
void chip8_emulate_cycle(Chip8 *c);
void chip8_set_key(Chip8 *c, uint8_t key, uint8_t pressed);
void chip8_clear_display(Chip8 *c);

#endif
