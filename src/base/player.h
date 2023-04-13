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
    double set_time; // Used if cannot actually output audio
    int playing;     // Used if cannot actually output audio
} player_t;

int player_init_file(player_t *, const char *);
int player_init_memory(player_t *, const unsigned char *, int len);
int player_at_end(player_t *player);
int player_is_playing(player_t *player);
double player_get_time(player_t *player);
void player_pause(player_t *player, int flag);
void player_set_time(player_t *player, double);
void player_destroy(player_t *player);

#endif
