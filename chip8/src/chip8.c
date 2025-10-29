#include "chip8.h"
#include <stdio.h>
#include <string.h>

/*
Concepts:
    Implementation of chip8.h functions for CHIP-8 emulator.
    Core initialization, ROM loading, input handling, display management, and emulation cycle.
    How is the CHIP-8 system initialized? By setting up memory, CPU, display, keys, and loading the fontset.
    How does ROM loading work? By reading a program file into the emulator's memory.
    How are keys managed? By setting or clearing their state in the keys array.
    How is the display cleared? By zeroing out the display buffer and setting the draw flag.
    What happens during an emulation cycle? Fetching, decoding, and executing an opcode.
*/


/* Initialize core subsystems */
void chip8_init(Chip8 *c) {
    memory_init(&c->memory);
    cpu_init(&c->cpu);
    memset(c->display, 0, sizeof(c->display));
    memset(c->keys, 0, sizeof(c->keys));
    c->draw_flag = 0;

    /* Load standard fontset into interpreter area (0x000 - 0x1FF).
       For now we use a minimal font (can be expanded). */
    static const uint8_t fontset[] = {
        0xF0,0x90,0x90,0x90,0xF0, /* 0 */
        0x20,0x60,0x20,0x20,0x70, /* 1 */
        0xF0,0x10,0xF0,0x80,0xF0, /* 2 */
        0xF0,0x10,0xF0,0x10,0xF0, /* 3 */
        0x90,0x90,0xF0,0x10,0x10, /* 4 */
        0xF0,0x80,0xF0,0x10,0xF0, /* 5 */
        0xF0,0x80,0xF0,0x90,0xF0, /* 6 */
        0xF0,0x10,0x20,0x40,0x40, /* 7 */
        0xF0,0x90,0xF0,0x90,0xF0, /* 8 */
        0xF0,0x90,0xF0,0x10,0xF0, /* 9 */
        0xF0,0x90,0xF0,0x90,0x90, /* A */
        0xE0,0x90,0xE0,0x90,0xE0, /* B */
        0xF0,0x80,0x80,0x80,0xF0, /* C */
        0xE0,0x90,0x90,0x90,0xE0, /* D */
        0xF0,0x80,0xF0,0x80,0xF0, /* E */
        0xF0,0x80,0xF0,0x80,0x80  /* F */
    };

    /* fontset load at 0x050 is common. adjust if you want another base. */
    const size_t font_offset = 0x50;
    memcpy(&c->memory.data[font_offset], fontset, sizeof(fontset));
}

/* Convenience wrapper for ROM loading */
void chip8_load_rom(Chip8 *c, const char *filename) {
    memory_load_rom(&c->memory, filename);
}

/* Set or clear a key (0..15) */
void chip8_set_key(Chip8 *c, uint8_t key, uint8_t pressed) {
    if (key < 16) c->keys[key] = pressed ? 1 : 0;
}

/* Clear the display buffer */
void chip8_clear_display(Chip8 *c) {
    memset(c->display, 0, sizeof(c->display));
    c->draw_flag = 1;
}

/*
 Emulate one fetch/decode/execute cycle.
 This function currently:
 - fetches the opcode (big-endian)
 - advances pc by 2
 - prints opcode for inspection
 - handles a couple minimal opcodes as examples
 Extend the switch to implement the rest of the CHIP-8 instruction set.
*/
void chip8_emulate_cycle(Chip8 *c) {
    uint16_t opcode = cpu_fetch_opcode(&c->cpu, &c->memory);

    /* Basic debug output. Remove or guard behind a debug flag later. */
    printf("Opcode: 0x%04X  PC: 0x%04X\n", opcode, c->cpu.pc);

    /* Decode example: use high nibble and more specific masks below */
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0: /* CLS: clear the display */
                    chip8_clear_display(c);
                    c->cpu.pc += 2;
                    break;
                case 0x00EE: /* RET: return from subroutine */
                    if (c->cpu.sp == 0) {
                        fprintf(stderr, "Stack underflow on RET\n");
                        c->cpu.pc += 2; /* attempt to continue */
                    } else {
                        c->cpu.sp--;
                        c->cpu.pc = c->cpu.stack[c->cpu.sp];
                        c->cpu.pc += 2;
                    }
                    break;
                default:
                    /* 0NNN is often ignored in modern interpreters */
                    c->cpu.pc += 2;
                    break;
            }
            break;

        case 0x1000: { /* 1NNN: JP addr */
            uint16_t addr = opcode & 0x0FFF;
            c->cpu.pc = addr;
            break;
        }

        case 0x2000: { /* 2NNN: CALL addr */
            uint16_t addr = opcode & 0x0FFF;
            if (c->cpu.sp >= 16) {
                fprintf(stderr, "Stack overflow on CALL\n");
            } else {
                c->cpu.stack[c->cpu.sp] = c->cpu.pc;
                c->cpu.sp++;
                c->cpu.pc = addr;
            }
            break;
        }

        case 0x6000: { /* 6XNN: LD Vx, byte */
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t byte = opcode & 0x00FF;
            c->cpu.V[x] = byte;
            c->cpu.pc += 2;
            break;
        }

        case 0xA000: { /* ANNN: LD I, addr */
            c->cpu.I = opcode & 0x0FFF;
            c->cpu.pc += 2;
            break;
        }

        case 0xD000: { /* DXYN: DRW Vx, Vy, nibble */
            /* For now implement a simple placeholder that sets draw_flag.
               Full sprite logic requires XOR and collision flag VF. */
            c->draw_flag = 1;
            c->cpu.pc += 2;
            break;
        }

        default:
            /* Unimplemented opcode: advance PC so we don't loop forever */
            c->cpu.pc += 2;
            break;
    }

    /* Timers should be updated at 60 Hz by the outer loop.
       This function does not modify them here. */
}
