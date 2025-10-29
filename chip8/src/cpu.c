#include "cpu.h"
/*
Concepts:
    Implementation of cpu.h functions for CHIP-8 emulator.
    CPU initialization and opcode fetching.
    How is the CPU initialized? By setting all registers, stack, and timers to zero, and setting the program counter to 0x200.
    What is opcode? It is a 2-byte instruction fetched from memory to be executed by the CPU.
    How is it fetched? By reading two consecutive bytes from memory at the current program counter and combining them.
*/
void cpu_init(CPU *c) {
    for (int i = 0; i < 16; i++) c->V[i] = 0;
    for (int i = 0; i < 16; i++) c->stack[i] = 0;
    c->I = 0;
    c->pc = 0x200;  // program start
    c->sp = 0;
    c->delay_timer = 0;
    c->sound_timer = 0;
}

uint16_t cpu_fetch_opcode(CPU *c, Memory *m) {
    uint8_t high = memory_read(m, c->pc);
    uint8_t low  = memory_read(m, c->pc + 1);
    return (high << 8) | low; // big-endian opcodes
}
