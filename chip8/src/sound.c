// sound.c

#include "sound.h"
#include <SDL2/SDL.h>
#include <math.h>

static SDL_AudioDeviceID audio_device = 0;
static uint8_t is_playing = 0;

// Audio callback function
void audio_callback(void *userdata, uint8_t *stream, int len) {
    (void)userdata;
    static float phase = 0.0f;
    const float frequency = 440.0f; // A note
    const float sample_rate = 44100.0f;
    const float amplitude = 0.1f; // Volume (0.0 to 1.0)
    
    int16_t *buffer = (int16_t*)stream;
    int samples = len / 2;
    
    for (int i = 0; i < samples; i++) {
        if (is_playing) {
            float sample = amplitude * sinf(phase * 2.0f * M_PI);
            buffer[i] = (int16_t)(sample * 32767.0f);
            phase += frequency / sample_rate;
            if (phase >= 1.0f) phase -= 1.0f;
        } else {
            buffer[i] = 0;
        }
    }
}

void sound_init(void) {
    SDL_AudioSpec want, have;
    SDL_memset(&want, 0, sizeof(want));
    
    want.freq = 44100;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = audio_callback;
    
    audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audio_device == 0) {
        fprintf(stderr, "Failed to open audio: %s\n", SDL_GetError());
        return;
    }
    
    SDL_PauseAudioDevice(audio_device, 0); // Start audio
}

void sound_play_beep(void) {
    is_playing = 1;
}

void sound_stop_beep(void) {
    is_playing = 0;
}

void sound_cleanup(void) {
    if (audio_device != 0) {
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
    }
}