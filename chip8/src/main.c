/*
Concepts:
    Main entry point for CHIP-8 emulator.
    Initializes memory and CPU, loads ROM, and fetches the first opcode.
    Demonstrates how to set up the emulator's core components and start execution.
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include "chip8.h"

static volatile int running = 1;

void handle_sigint(int sig) {
    (void)sig;
    running = 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, handle_sigint);

    Chip8 sys;
    chip8_init(&sys);
    chip8_load_rom(&sys, argv[1]);

    // Simple main loop:
    // Run many CPU cycles per frame. Timers update at ~60Hz.
    const int cycles_per_frame = 10;
    const struct timespec frame_ns = {0, 16666667}; // ~60Hz

    struct timespec last_timer;
    clock_gettime(CLOCK_MONOTONIC, &last_timer);

    while (running) {
        for (int i = 0; i < cycles_per_frame; ++i) {
            chip8_emulate_cycle(&sys);
        }

        // If draw flag set, render ASCII framebuffer
        if (sys.draw_flag) {
            chip8_draw_display(&sys);
            sys.draw_flag = 0;
        }

        // Timer update at ~60Hz
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double diff = (now.tv_sec - last_timer.tv_sec) + (now.tv_nsec - last_timer.tv_nsec) / 1e9;
        if (diff >= (1.0 / 60.0)) {
            if (sys.cpu.delay_timer > 0) sys.cpu.delay_timer--;
            if (sys.cpu.sound_timer > 0) sys.cpu.sound_timer--;
            last_timer = now;
        }

        // small sleep to cap speed
        nanosleep(&frame_ns, NULL);
    }

    printf("\nExiting emulator.\n");
    return 0;
}
