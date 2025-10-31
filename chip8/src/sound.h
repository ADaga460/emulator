// sound.h

#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

void sound_init(void);
void sound_play_beep(void);
void sound_stop_beep(void);
void sound_cleanup(void);

#endif