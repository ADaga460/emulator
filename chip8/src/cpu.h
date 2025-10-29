#ifndef CPU_H
#define CPU_H
#include <stdint.h>
#include "memory.h"
/*
Concepts:
    CHIP-8 CPU has:
    - 16 general-purpose 8-bit registers (V0 to VF)
    - 1 16-bit address register (I)
    - 1 16-bit program counter (pc)
    - 1 stack for subroutine calls (16 levels)
    - 1 stack pointer (sp)
    - 2 timers (delay and sound)
    This structure models the CPU state for emulation.
*/
typedef struct {
    uint8_t  V[16];      // general registers
    uint16_t I;          // address register
    uint16_t pc;         // program counter
    uint16_t stack[16];  // call stack
    uint8_t  sp;         // stack pointer
    uint8_t  delay_timer;
    uint8_t  sound_timer;
} CPU;

void cpu_init(CPU *c);
uint16_t cpu_fetch_opcode(CPU *c, Memory *m);

#endif
