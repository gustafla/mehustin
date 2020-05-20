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
    SDL_AudioDeviceID audio_device;
    SDL_AudioSpec spec;
    playback_t playback;
} player_t;

player_t *player_init(const char*);
int player_at_end(player_t *player);
double player_get_time(player_t *player);
void player_set_time(player_t *player, double);
void player_free(player_t *player);

#endif
