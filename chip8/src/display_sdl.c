//display_sdl.c

#include "display_sdl.h"
#include "SDL.h"
#include <stdlib.h>

Display* display_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return NULL;
    }
    
    Display *d = malloc(sizeof(Display));
    if (!d) return NULL;
    
    d->window = SDL_CreateWindow("CHIP-8 Emulator", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 320, SDL_WINDOW_SHOWN);
    
    if (!d->window) {
        free(d);
        return NULL;
    }
    
    d->renderer = SDL_CreateRenderer(d->window, -1, SDL_RENDERER_ACCELERATED);
    if (!d->renderer) {
        SDL_DestroyWindow(d->window);
        free(d);
        return NULL;
    }
    
    d->texture = SDL_CreateTexture(d->renderer,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
        64, 32);
    
    if (!d->texture) {
        SDL_DestroyRenderer(d->renderer);
        SDL_DestroyWindow(d->window);
        free(d);
        return NULL;
    }
    
    return d;
}

void display_render(Display *d, const uint8_t *gfx) {
    uint32_t pixels[64 * 32];
    for (int i = 0; i < 64 * 32; i++) {
        pixels[i] = gfx[i] ? 0xFFFFFFFF : 0x00000000;
    }
    
    SDL_UpdateTexture(d->texture, NULL, pixels, 64 * sizeof(uint32_t));
    SDL_RenderClear(d->renderer);
    SDL_RenderCopy(d->renderer, d->texture, NULL, NULL);
    SDL_RenderPresent(d->renderer);
}

void display_cleanup(Display *d) {
    if (d) {
        if (d->texture) SDL_DestroyTexture(d->texture);
        if (d->renderer) SDL_DestroyRenderer(d->renderer);
        if (d->window) SDL_DestroyWindow(d->window);
        free(d);
    }
    SDL_Quit();
}

int display_handle_input(Display *d, uint8_t *keys) {
    (void)d; // unused
    SDL_Event e;
    
    // Clear all keys first
    for (int i = 0; i < 16; i++) {
        keys[i] = 0;
    }
    
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return 0; // quit
        }
    }
    
    // Check current keyboard state
    const uint8_t *state = SDL_GetKeyboardState(NULL);
    
    // Map keyboard to CHIP-8 keys
    // CHIP-8:  1 2 3 C     Keyboard:  1 2 3 4
    //          4 5 6 D                Q W E R
    //          7 8 9 E                A S D F
    //          A 0 B F                Z X C V
    
    if (state[SDL_SCANCODE_1]) keys[0x1] = 1;
    if (state[SDL_SCANCODE_2]) keys[0x2] = 1;
    if (state[SDL_SCANCODE_3]) keys[0x3] = 1;
    if (state[SDL_SCANCODE_4]) keys[0xC] = 1;
    if (state[SDL_SCANCODE_Q]) keys[0x4] = 1;
    if (state[SDL_SCANCODE_W]) keys[0x5] = 1;
    if (state[SDL_SCANCODE_E]) keys[0x6] = 1;
    if (state[SDL_SCANCODE_R]) keys[0xD] = 1;
    if (state[SDL_SCANCODE_A]) keys[0x7] = 1;
    if (state[SDL_SCANCODE_S]) keys[0x8] = 1;
    if (state[SDL_SCANCODE_D]) keys[0x9] = 1;
    if (state[SDL_SCANCODE_F]) keys[0xE] = 1;
    if (state[SDL_SCANCODE_Z]) keys[0xA] = 1;
    if (state[SDL_SCANCODE_X]) keys[0x0] = 1;
    if (state[SDL_SCANCODE_C]) keys[0xB] = 1;
    if (state[SDL_SCANCODE_V]) keys[0xF] = 1;
    if (state[SDL_SCANCODE_ESCAPE]) return 0; // quit
    
    return 1; // continue running
}