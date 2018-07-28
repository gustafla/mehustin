#include "demo.h"
#include <SDL.h>

#define GET(x) demo_sync_get_value(demo, x)

// constants for rocket sync
static const float BPM = 120; // beats per minute
static const int RPB = 8; // rows per beat
static const float ROW_RATE = (BPM / 60.f) * RPB; // rows per second

// Rocket editor sync with music player
#ifndef SYNC_PLAYER
static void player_pause(void *d, int flag) {
    player_t *player = (player_t*)d;
    SDL_PauseAudioDevice(player->audio_device, flag);
}

static void player_set_row(void *d, int row) {
    player_t *player = (player_t*)d;
    // desired row to byte
    size_t pos = row / ROW_RATE * player->spec.channels * sizeof(Uint16) * player->spec.freq;
    // align byte position to first byte of left sample
    pos -= pos % (player->spec.channels * sizeof(Uint16));
    // update player position
    SDL_LockAudioDevice(player->audio_device);
    player->playback.pos = pos;
    SDL_UnlockAudioDevice(player->audio_device);
}

static int player_is_playing(void *d) {
    player_t *player = (player_t*)d;
    return SDL_GetAudioDeviceStatus(player->audio_device) == SDL_AUDIO_PLAYING;
}

static struct sync_cb player_cb = {
    player_pause,
    player_set_row,
    player_is_playing
};
#endif

demo_t *demo_init(player_t *player) {
    demo_t *demo = calloc(1, sizeof(demo_t));
    if (!demo) return NULL;

    // Init rocket
    demo->rocket = sync_create_device("sync");
    if (!demo->rocket) {
        fprintf(stderr, "sync_create_device failed\n");
        return NULL;
    }

    // Connect rocket
#ifndef SYNC_PLAYER
    if (sync_tcp_connect(demo->rocket, "localhost", SYNC_DEFAULT_PORT)) {
        fprintf(stderr, "sync_tcp_connect failed\n");
        return NULL;
    }
#endif

    // Start music
    demo->player = player;
    SDL_PauseAudioDevice(player->audio_device, 0);
    return demo;
}

void demo_free(demo_t *demo) {
    // save tracks to librocket-player format
#ifndef SYNC_PLAYER
    sync_save_tracks(demo->rocket);
#endif
    sync_destroy_device(demo->rocket);
    free(demo);
}

void demo_render(demo_t *demo) {
    // get time from player and convert to rocket row
    player_t *player = demo->player;
    SDL_LockAudioDevice(player->audio_device);
    float byte_at = player->playback.pos;
    SDL_UnlockAudioDevice(player->audio_device);
    demo->time = byte_at / player->spec.channels / sizeof(Uint16) / player->spec.freq;
    demo->row = demo->time * ROW_RATE;

    // update rocket
#ifndef SYNC_PLAYER
    if (sync_update(demo->rocket, (int)floor(demo->row), &player_cb, (void*)player)) {
        sync_tcp_connect(demo->rocket, "localhost", SYNC_DEFAULT_PORT);
    }
#endif

    // render
    printf("%f\n", GET("test"));
}

void demo_recompile(demo_t *demo) {
    
}

float demo_sync_get_value(demo_t* demo, const char *name) {
    // ugly because sync_get_track takes long, should keep tracks cached
    return sync_get_val(sync_get_track(demo->rocket, name), demo->row);
}
