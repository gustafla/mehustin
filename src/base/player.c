#include "player.h"
#include <stdio.h>
#include <unistd.h>
#include "stb_vorbis.c"

inline int min(int a, int b) {
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
    // Update precise time
    playback->call_time = SDL_GetTicks();
    // no mixing needed
    memcpy(stream, playback->data + playback->pos, len);
    // update position
    playback->pos += len;
}

player_t *player_init(const char *vorbis_file_path) {
    player_t *player = calloc(1, sizeof(player_t));
    if (!player) return NULL;

    // decode vorbis file
    short *audio;
    int channels, sample_rate;
    int samples = stb_vorbis_decode_filename(vorbis_file_path, &channels,
            &sample_rate, &audio);
    if (samples < 1) {
        fprintf(stderr, "Failed to open vorbis file %s\n", vorbis_file_path);
        fprintf(stderr, "Starting without audio (end with ESC or q)\n");
        sleep(2);
    } else {
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
        player->audio_device = SDL_OpenAudioDevice(
                NULL,
                0,
                &desired,
                &player->spec,
                0);

        if (!player->audio_device) {
            fprintf(stderr, "Failed to open audio device:\n%s\n", SDL_GetError());
            fprintf(stderr, "Starting without audio (end with ESC or q)\n");
            sleep(2);
        }
    }

    return player;
}

int player_at_end(player_t *player) {
    if (player->audio_device) {
        return player->playback.pos >= player->playback.bytes;
    }

    return 0;
}

int player_is_playing(player_t *player) {
    if (player->audio_device) {
        return SDL_GetAudioDeviceStatus(player->audio_device) == SDL_AUDIO_PLAYING;
    }

    return player->playing;
}

double player_get_time(player_t *player) {
    double time = 0;

    if (player->audio_device) {
        SDL_LockAudioDevice(player->audio_device);
        double byte_at = player->playback.pos;
        SDL_UnlockAudioDevice(player->audio_device);
        time = byte_at / player->spec.channels / sizeof(Uint16)
            / player->spec.freq;
    } else {
        time = player->set_time;
    }

    // add precision
    if (player_is_playing(player)) {
        time += (SDL_GetTicks() - player->playback.call_time) / 1000.;
    }

    return time;
}

void player_pause(player_t *player, int flag) {
    if (player->audio_device) {
        SDL_LockAudioDevice(player->audio_device);
        player->playback.call_time = SDL_GetTicks();
        SDL_UnlockAudioDevice(player->audio_device);
        SDL_PauseAudioDevice(player->audio_device, flag);
    } else {
        player->set_time = player_get_time(player);
        player->playback.call_time = SDL_GetTicks();
        player->playing = !flag;
    }
}

void player_set_time(player_t *player, double time) {
    if (player->audio_device) {
        // desired time to byte
        size_t pos = time * player->spec.channels * sizeof(Uint16)
            * player->spec.freq;
        // align byte position to first byte of left sample
        pos -= pos % (player->spec.channels * sizeof(Uint16));
        // update player position
        SDL_LockAudioDevice(player->audio_device);
        player->playback.pos = pos;
        SDL_UnlockAudioDevice(player->audio_device);
    } else {
        player->set_time = time;
        player->playback.call_time = SDL_GetTicks();
    }
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
