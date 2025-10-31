// main.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"
#include "display_sdl.h"
#include "sound.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom>\n", argv[0]);
        return 1;
    }

    // Initialize emulator
    Chip8 sys;
    chip8_init(&sys);
    chip8_load_rom(&sys, argv[1]);

    // Initialize display and sound
    Display *display = display_init();
    if (!display) {
        fprintf(stderr, "Failed to initialize display\n");
        return 1;
    }
    sound_init();

    // Main loop variables
    const int cycles_per_frame = 10;
    const double target_frame_time = 1.0 / 60.0; // 60 FPS
    
    struct timespec last_timer, frame_start;
    clock_gettime(CLOCK_MONOTONIC, &last_timer);

    int running = 1;
    while (running) {
        clock_gettime(CLOCK_MONOTONIC, &frame_start);

        // Handle input
        running = display_handle_input(display, sys.keys);

        // Run CPU cycles
        for (int i = 0; i < cycles_per_frame; ++i) {
            chip8_emulate_cycle(&sys);
        }

        // Render if draw flag is set
        if (sys.draw_flag) {
            display_render(display, sys.gfx);
            sys.draw_flag = 0;
        }

        // Update timers at 60Hz
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double diff = (now.tv_sec - last_timer.tv_sec) + 
                     (now.tv_nsec - last_timer.tv_nsec) / 1e9;
        
        if (diff >= (1.0 / 60.0)) {
            if (sys.cpu.delay_timer > 0) sys.cpu.delay_timer--;
            
            if (sys.cpu.sound_timer > 0) {
                sound_play_beep();
                sys.cpu.sound_timer--;
            } else {
                sound_stop_beep();
            }
            
            last_timer = now;
        }

        // Cap frame rate
        clock_gettime(CLOCK_MONOTONIC, &now);
        double frame_time = (now.tv_sec - frame_start.tv_sec) + 
                           (now.tv_nsec - frame_start.tv_nsec) / 1e9;
        
        if (frame_time < target_frame_time) {
            struct timespec sleep_time;
            double sleep_sec = target_frame_time - frame_time;
            sleep_time.tv_sec = (time_t)sleep_sec;
            sleep_time.tv_nsec = (long)((sleep_sec - sleep_time.tv_sec) * 1e9);
            nanosleep(&sleep_time, NULL);
        }
    }

    // Cleanup
    sound_cleanup();
    display_cleanup(display);
    printf("Exiting emulator.\n");
    return 0;
}