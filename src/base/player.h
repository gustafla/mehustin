#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>

typedef struct {
    Uint8 *data;
    size_t bytes;
    size_t pos;
    unsigned call_time;
} playback_t;

typedef struct {
    double row_rate; // visuals sync speed in rows/s
    SDL_AudioDeviceID audio_device;
    SDL_AudioSpec spec;
    playback_t playback;
} player_t;

player_t *player_init(const char*, double, double);
int player_at_end(player_t *player);
void player_free(player_t *player);

#endif
