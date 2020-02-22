#include "player.h"
#include <stdio.h>
#include "stb_vorbis.c"

static int min(int a, int b) {
    if (a < b) return a;
    return b;
}

static void callback(void *userdata, Uint8 *stream, int len) {
    playback_t *playback = (playback_t*)userdata;
    // play silence if end is reached
    memset(stream, 0, len);
    // only play as much as there is audio data left
    len = min(len, playback->bytes - playback->pos);
    // stop here if end reached
    if (len <= 0) return;
    // no mixing needed
    memcpy(stream, playback->data + playback->pos, len);
    // update position
    playback->pos += len;
}

player_t *player_init(const char *vorbis_file_path, double bpm, double rpb) {
    player_t *player = calloc(1, sizeof(player_t));
    if (!player) return NULL;

    // decode vorbis file
    short *audio;
    int channels, sample_rate;
    int samples = stb_vorbis_decode_filename(vorbis_file_path, &channels,
            &sample_rate, &audio);
    if (samples < 1) {
        fprintf(stderr, "Failed to open vorbis file %s\n", vorbis_file_path);
        player_free(player);
        return NULL;
    }

    // cast audio to bytes for playback and compute size in bytes
    player->playback.data = (Uint8*)audio; // char* can point to anything
    player->playback.bytes = samples * channels * sizeof(short);

    // this spec needed to play the file back
    SDL_AudioSpec desired = {
        .freq = sample_rate,
        .format = AUDIO_S16SYS,
        .channels = channels,
        .samples = 4096,
        .callback = callback,
        .userdata = (void*)&player->playback
    };

    // prepare the audio hardware
    player->audio_device = SDL_OpenAudioDevice(NULL, 0, &desired, &player->spec,
            0);
    if (!player->audio_device) {
        fprintf(stderr, "Failed to open audio device: %s\n", SDL_GetError());
        player_free(player);
        return NULL;
    }

    // set row rate
    player->row_rate = (bpm / 60.) * rpb;

    return player;
}

int player_at_end(player_t *player) {
    return player->playback.pos >= player->playback.bytes;
}

void player_free(player_t *player) {
    if (player->audio_device) {
        SDL_CloseAudioDevice(player->audio_device);
    }
    if (player->playback.data) {
        free(player->playback.data);
    }
    free(player);
}
