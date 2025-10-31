// chip8.c

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
#include "chip8.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Minimal fontset, 5 bytes per character 0-F, commonly loaded at 0x50 */
static const uint8_t fontset[80] = {
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

void chip8_init(Chip8 *c) {
    memory_init(&c->memory);
    cpu_init(&c->cpu);
    memset(c->gfx, 0, sizeof(c->gfx));
    memset(c->keys, 0, sizeof(c->keys));
    c->draw_flag = 0;

    /* Load fontset at 0x50 */
    const size_t fontaddr = 0x50;
    if (fontaddr + sizeof(fontset) < MEMORY_SIZE) {
        memcpy(&c->memory.data[fontaddr], fontset, sizeof(fontset));
    }
}

void chip8_load_rom(Chip8 *c, const char *filename) {
    memory_load_rom(&c->memory, filename);
}

void chip8_set_key(Chip8 *c, uint8_t key, uint8_t pressed) {
    if (key < 16) c->keys[key] = pressed ? 1 : 0;
}

void chip8_clear_display(Chip8 *c) {
    memset(c->gfx, 0, sizeof(c->gfx));
    c->draw_flag = 1;
}

void chip8_draw_display(const Chip8 *c) {
    // clear screen ANSI
    printf("\033[H\033[J");
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            putchar(c->gfx[y * DISPLAY_WIDTH + x] ? '#' : ' ');
        }
        putchar('\n');
    }
    fflush(stdout);  // ADD THIS LINE
}

/* Emulate one CPU cycle: fetch, decode, execute */
void chip8_emulate_cycle(Chip8 *c) {
    uint16_t opcode = cpu_fetch_opcode(&c->cpu, &c->memory);
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t nn = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t n = opcode & 0x000F;

    // Debug
    //printf("Opcode: 0x%04X  PC: 0x%04X\n", opcode, c->cpu.pc);

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x00FF) {
                case 0x00E0: // CLS
                    chip8_clear_display(c);
                    c->cpu.pc += 2;
                    break;
                case 0x00EE: // RET
                    if (c->cpu.sp == 0) {
                        fprintf(stderr, "Stack underflow on RET\n");
                        c->cpu.pc += 2;
                    } else {
                        c->cpu.sp--;
                        c->cpu.pc = c->cpu.stack[c->cpu.sp];  // Just restore, no +2
                    }
                    break;
                default:
                    // 0NNN ignored
                    c->cpu.pc += 2;
                    break;
            }
            break;

        case 0x1000: // JP addr
            c->cpu.pc = nnn;
            break;

        case 0x2000: // CALL addr
            if (c->cpu.sp < 16) {
                c->cpu.stack[c->cpu.sp++] = c->cpu.pc + 2;  // Save return address
                c->cpu.pc = nnn;
            } else {
                fprintf(stderr, "Stack overflow on CALL\n");
            }
            break;

        case 0x3000: // SE Vx, byte
            c->cpu.pc += (c->cpu.V[x] == nn) ? 4 : 2;
            break;

        case 0x4000: // SNE Vx, byte
            c->cpu.pc += (c->cpu.V[x] != nn) ? 4 : 2;
            break;

        case 0x5000: // SE Vx, Vy (5XY0)
            if ((opcode & 0x000F) == 0x0) {
                c->cpu.pc += (c->cpu.V[x] == c->cpu.V[y]) ? 4 : 2;
            } else {
                c->cpu.pc += 2;
            }
            break;

        case 0x6000: // LD Vx, byte
            c->cpu.V[x] = nn;
            c->cpu.pc += 2;
            break;

        case 0x7000: // ADD Vx, byte
            c->cpu.V[x] = (uint8_t)(c->cpu.V[x] + nn);
            c->cpu.pc += 2;
            break;

        case 0x8000: {
            uint8_t sub = opcode & 0x000F;
            switch (sub) {
                case 0x1: // OR Vx, Vy
                c->cpu.V[x] |= c->cpu.V[y];
                break;
            case 0x2: // AND Vx, Vy
                c->cpu.V[x] &= c->cpu.V[y];
                break;
            case 0x3: // XOR Vx, Vy
                c->cpu.V[x] ^= c->cpu.V[y];
                break;
            case 0x5: { // SUB Vx, Vy; VF = NOT borrow
                c->cpu.V[0xF] = (c->cpu.V[x] >= c->cpu.V[y]) ? 1 : 0;
                c->cpu.V[x] -= c->cpu.V[y];
                break;
            }
            case 0x6: { // SHR Vx {, Vy}
                c->cpu.V[0xF] = c->cpu.V[x] & 0x1;
                c->cpu.V[x] >>= 1;
                break;
            }
            case 0x7: { // SUBN Vx, Vy; VF = NOT borrow
                c->cpu.V[0xF] = (c->cpu.V[y] >= c->cpu.V[x]) ? 1 : 0;
                c->cpu.V[x] = c->cpu.V[y] - c->cpu.V[x];
                break;
            }
            case 0xE: { // SHL Vx {, Vy}
                c->cpu.V[0xF] = (c->cpu.V[x] & 0x80) >> 7;
                c->cpu.V[x] <<= 1;
                break;
            }
                default:
                    // not implemented other 8XYn
                    break;
            }
            c->cpu.pc += 2;
            break;
        }
        case 0x9000: // SNE Vx, Vy (9XY0)
            if ((opcode & 0x000F) == 0x0) {
                c->cpu.pc += (c->cpu.V[x] != c->cpu.V[y]) ? 4 : 2;
            } else {
                c->cpu.pc += 2;
            }
            break;
        case 0xA000: // LD I, addr
            c->cpu.I = nnn;
            c->cpu.pc += 2;
            break;

        case 0xB000: // JP V0, addr
            c->cpu.pc = nnn + c->cpu.V[0];
            break;

        case 0xC000: { // RND Vx, byte
            uint8_t rnd = (uint8_t)(rand() & 0xFF);
            c->cpu.V[x] = rnd & nn;
            c->cpu.pc += 2;
            break;
        }

        case 0xD000: { // DRW Vx, Vy, nibble
            uint8_t xPos = c->cpu.V[x] % DISPLAY_WIDTH;
            uint8_t yPos = c->cpu.V[y] % DISPLAY_HEIGHT;
            uint8_t height = n;
            c->cpu.V[0xF] = 0;

            for (uint8_t row = 0; row < height; ++row) {
                uint16_t sprite_addr = c->cpu.I + row;
                if (sprite_addr >= MEMORY_SIZE) break;
                uint8_t sprite = c->memory.data[sprite_addr];
                for (uint8_t col = 0; col < 8; ++col) {
                    if ((sprite & (0x80 >> col)) != 0) {
                        uint16_t px = (xPos + col) % DISPLAY_WIDTH;
                        uint16_t py = (yPos + row) % DISPLAY_HEIGHT;
                        uint16_t idx = py * DISPLAY_WIDTH + px;

                        if (c->gfx[idx] == 1) c->cpu.V[0xF] = 1;
                        c->gfx[idx] ^= 1;
                    }
                }
            }

            c->draw_flag = 1;
            c->cpu.pc += 2;
            break;
        }

        case 0xE000: { // key opcodes
            switch (opcode & 0x00FF) {
                case 0x9E: // SKP Vx
                    c->cpu.pc += (c->keys[c->cpu.V[x]] ? 4 : 2);
                    break;
                case 0xA1: // SKNP Vx
                    c->cpu.pc += (c->keys[c->cpu.V[x]] ? 2 : 4);
                    break;
                default:
                    c->cpu.pc += 2;
                    break;
            }
            break;
        }

        case 0xF000: {
            switch (opcode & 0x00FF) {
                case 0x07: // LD Vx, DT
                    c->cpu.V[x] = c->cpu.delay_timer;
                    c->cpu.pc += 2;
                    break;
                case 0x15: // LD DT, Vx
                    c->cpu.delay_timer = c->cpu.V[x];
                    c->cpu.pc += 2;
                    break;
                case 0x18: // LD ST, Vx
                    c->cpu.sound_timer = c->cpu.V[x];
                    c->cpu.pc += 2;
                    break;
                case 0x1E: // ADD I, Vx
                    c->cpu.I += c->cpu.V[x];
                    c->cpu.pc += 2;
                    break;
                case 0x0A: { // LD Vx, K (blocking wait)
                    uint8_t found = 0;
                    for (uint8_t i = 0; i < 16; ++i) {
                        if (c->keys[i]) {
                            c->cpu.V[x] = i;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        // do not advance PC, wait for key
                    } else {
                        c->cpu.pc += 2;
                    }
                    break;
                }
                case 0x29: // LD F, Vx (set I to font sprite for digit Vx)
                    c->cpu.I = 0x50 + (c->cpu.V[x] * 5);
                    c->cpu.pc += 2;
                    break;
                case 0x33: { // LD B, Vx (store BCD)
                    uint8_t val = c->cpu.V[x];
                    c->memory.data[c->cpu.I + 0] = val / 100;
                    c->memory.data[c->cpu.I + 1] = (val / 10) % 10;
                    c->memory.data[c->cpu.I + 2] = val % 10;
                    c->cpu.pc += 2;
                    break;
                }
                case 0x55: { // LD [I], V0..Vx
                    for (uint8_t i = 0; i <= x; ++i) {
                        c->memory.data[c->cpu.I + i] = c->cpu.V[i];
                    }
                    c->cpu.pc += 2;
                    break;
                }
                case 0x65: { // LD V0..Vx, [I]
                    for (uint8_t i = 0; i <= x; ++i) {
                        c->cpu.V[i] = c->memory.data[c->cpu.I + i];
                    }
                    c->cpu.pc += 2;
                    break;
                }
                default:
                    c->cpu.pc += 2;
                    break;
            }
            break;
        }

        default:
            // unimplemented. advance to avoid infinite loop
            c->cpu.pc += 2;
            break;
    }
}
