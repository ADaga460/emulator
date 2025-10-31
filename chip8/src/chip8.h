// chip8.h

#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include "memory.h"
#include "cpu.h"

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define DISPLAY_SIZE (DISPLAY_WIDTH * DISPLAY_HEIGHT)

typedef struct {
    Memory memory;
    CPU cpu;
    uint8_t gfx[DISPLAY_SIZE]; // 0/1 pixels
    uint8_t keys[16];          // hex keypad state
    uint8_t draw_flag;         // set when display changed
} Chip8;

void chip8_init(Chip8 *c);
void chip8_load_rom(Chip8 *c, const char *filename);
void chip8_emulate_cycle(Chip8 *c);
void chip8_set_key(Chip8 *c, uint8_t key, uint8_t pressed);
void chip8_clear_display(Chip8 *c);
void chip8_draw_display(const Chip8 *c);

#endif
