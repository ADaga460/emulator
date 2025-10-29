#include <stdio.h>
#include "memory.h"
#include "cpu.h"
#include "chip8.h"
/*
Concepts:
    Main entry point for CHIP-8 emulator.
    Initializes memory and CPU, loads ROM, and fetches the first opcode.
    Demonstrates how to set up the emulator's core components and start execution.
*/

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom>\n", argv[0]);
        return 1;
    }

    Chip8 system;
    chip8_init(&system);
    chip8_load_rom(&system, argv[1]);

    /* Single-cycle demo: fetch a few opcodes and run them.
       Replace with a proper loop and timing later. */
    for (int i = 0; i < 8; ++i) {
        chip8_emulate_cycle(&system);
    }

    return 0;
}
