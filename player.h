#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

typedef struct {
    Uint8 *data;
    size_t bytes;
    size_t pos;
} playback_t;

typedef struct {
    SDL_AudioDeviceID audio_device;
    SDL_AudioSpec spec;
    playback_t playback;
} player_t;

player_t *player_init(const char*);
void player_free(player_t *player);

#endif
