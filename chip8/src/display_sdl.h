// display_sdl.h

#ifndef DISPLAY_SDL_H
#define DISPLAY_SDL_H

#include <stdint.h>
#include <SDL2/SDL.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Display;

Display* display_init(void);
void display_render(Display *d, const uint8_t *gfx);
void display_cleanup(Display *d);
int display_handle_input(Display *d, uint8_t *keys);

#endif