#include "player.h"
#include <stdio.h>
#include <unistd.h>

inline int min(int a, int b) {
    if (a < b)
        return a;
    return b;
}

static void callback(void *userdata, Uint8 *stream, int len) {
    playback_t *playback = (playback_t *)userdata;
    bzero(stream, len);
    int num_floats = len / sizeof(float);
    if (stb_vorbis_get_samples_float_interleaved(
            playback->vorbis, playback->channels, (float *)stream,
            num_floats) == 0) {
        playback->at_end = 1;
    }
    // Update precise time
    playback->call_time = SDL_GetTicks();
}

int player_init(player_t *player, stb_vorbis *vorbis) {
    if (!player)
        return -1;

    bzero(player, sizeof(player_t));

    if (vorbis) {
        stb_vorbis_info info = stb_vorbis_get_info(vorbis);
        player->playback.vorbis = vorbis;
        player->playback.channels = info.channels;

        // this spec needed to play the file back
        SDL_AudioSpec desired = {.freq = info.sample_rate,
                                 .format = AUDIO_F32SYS,
                                 .channels = info.channels,
                                 .samples = 4096,
                                 .callback = callback,
                                 .userdata = (void *)&player->playback};

        // prepare the audio hardware
        player->audio_device =
            SDL_OpenAudioDevice(NULL, 0, &desired, &player->spec, 0);

        if (!player->audio_device) {
            fprintf(stderr, "Failed to open audio device:\n%s\n",
                    SDL_GetError());
        }
    }

    if (!player->audio_device) {
        fprintf(stderr, "Starting without audio (end with ESC or q)\n");
        sleep(2);
    }

    return 0;
}

int player_init_file(player_t *player, const char *filename) {
    int error = VORBIS__no_error;
    stb_vorbis *vorbis = stb_vorbis_open_filename(filename, &error, NULL);

    if (error != VORBIS__no_error) {
        vorbis = NULL;
        fprintf(stderr, "Failed to open vorbis file %s\n", filename);
    }

    return player_init(player, vorbis);
}

int player_init_memory(player_t *player, const unsigned char *data, int len) {
    int error = VORBIS__no_error;
    stb_vorbis *vorbis = stb_vorbis_open_memory(data, len, &error, NULL);

    if (error != VORBIS__no_error) {
        return -1;
    }

    return player_init(player, vorbis);
}

int player_at_end(player_t *player) {
    if (player->audio_device) {
        return player->playback.at_end;
    }

    return 0;
}

int player_is_playing(player_t *player) {
    if (player->audio_device) {
        return SDL_GetAudioDeviceStatus(player->audio_device) ==
               SDL_AUDIO_PLAYING;
    }

    return player->playing;
}

double player_get_time(player_t *player) {
    double time = 0;

    if (player->audio_device) {
        SDL_LockAudioDevice(player->audio_device);
        double sample = stb_vorbis_get_sample_offset(player->playback.vorbis);
        SDL_UnlockAudioDevice(player->audio_device);
        time = sample / player->spec.freq;
    } else {
        time = player->time;
    }

    // add precision
    if (player_is_playing(player) && !player_at_end(player)) {
        time += (SDL_GetTicks() - player->playback.call_time) / 1000.;
    }

    return time;
}

void player_pause(player_t *player, int flag) {
    if (player->audio_device) {
        player->playback.call_time = SDL_GetTicks();
        SDL_PauseAudioDevice(player->audio_device, flag);
    } else {
        player->time = player_get_time(player);
        player->playback.call_time = SDL_GetTicks();
        player->playing = !flag;
    }
}

void player_set_time(player_t *player, double time) {
    if (player->audio_device) {
        size_t sample = time * player->spec.freq;
        SDL_LockAudioDevice(player->audio_device);
        stb_vorbis_seek(player->playback.vorbis, sample);
        player->playback.at_end = 0;
        SDL_UnlockAudioDevice(player->audio_device);
    } else {
        player->time = time;
        player->playback.call_time = SDL_GetTicks();
    }
}

void player_destroy(player_t *player) {
    if (player->audio_device) {
        SDL_CloseAudioDevice(player->audio_device);
    }
    if (player->playback.vorbis) {
        stb_vorbis_close(player->playback.vorbis);
    }
}
