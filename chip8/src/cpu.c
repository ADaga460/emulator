// cpu.c

/*
Concepts:
    Implementation of cpu.h functions for CHIP-8 emulator.
    CPU initialization and opcode fetching.
    How is the CPU initialized? By setting all registers, stack, and timers to zero, and setting the program counter to 0x200.
    What is opcode? It is a 2-byte instruction fetched from memory to be executed by the CPU.
    How is it fetched? By reading two consecutive bytes from memory at the current program counter and combining them.
*/

#include "cpu.h"
#include <string.h>

void cpu_init(CPU *c) {
    memset(c->V, 0, sizeof(c->V));
    c->I = 0;
    c->pc = ROM_START; // start at 0x200
    memset(c->stack, 0, sizeof(c->stack));
    c->sp = 0;
    c->delay_timer = 0;
    c->sound_timer = 0;
}

uint16_t cpu_fetch_opcode(CPU *c, Memory *m) {
    uint8_t hi = memory_read(m, c->pc);
    uint8_t lo = memory_read(m, c->pc + 1);
    return (uint16_t)((hi << 8) | lo); // big-endian opcode
}
