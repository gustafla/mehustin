#include "demo.h"
#include <SDL.h>

demo_t *demo_init(player_t *player) {
    demo_t *demo = malloc(sizeof(demo_t));
    if (!demo) return NULL;
    memset(demo, 0, sizeof(demo_t));
    demo->player = player;
    SDL_PauseAudioDevice(player->audio_device, 0);
    return demo;
}

void demo_free(demo_t *demo) {
    free(demo);
}

void demo_render(demo_t *demo) {
    // get time from player
    player_t *player = demo->player;
    SDL_LockAudioDevice(player->audio_device);
    demo->time = ((float)player->playback.pos) / player->spec.channels / sizeof(short) / player->spec.freq;
    SDL_UnlockAudioDevice(player->audio_device);
    printf("%.2f\n", demo->time);
}

void demo_recompile(demo_t *demo) {
    
}
