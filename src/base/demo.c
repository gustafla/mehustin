#include "demo.h"
#include "sync.h"
#include <stdint.h>
#include <SDL.h>
#include <unistd.h>

#ifdef DEMO_RTDL
#include <dlfcn.h>
#include <sys/wait.h>
static const char *MODULE_PATH = "./libdemo.so";
#else
#include "src/scene/scene.h"
#endif

static struct demo_ {
    double time;
    double row;
    player_t *player;
    struct sync_device *rocket;
    int width;
    int height;
    void *scene_data;

    // scene module variables
#ifdef DEMO_RTDL
    void *module;
    // width, height, getval("...")
    void *(*scene_init)(
            int32_t,
            int32_t,
            const void *(*)(const char*),
            double (*)(const void*));
    // data
    void (*scene_deinit)(void*);
    // time, data
    void (*scene_render)(double, void*);
#endif
} demo;

// rocket editor sync with music player
#ifndef SYNC_PLAYER
static void player_pause(void *d, int flag) {
    player_t *player = (player_t*)d;
    SDL_PauseAudioDevice(player->audio_device, flag);
}

static void player_set_row(void *d, int row) {
    player_t *player = (player_t*)d;
    // desired row to byte
    size_t pos = row / player->row_rate * player->spec.channels * sizeof(Uint16)
        * player->spec.freq;
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

int demo_init(player_t *player, int width, int height) {
    // init rocket
    demo.rocket = sync_create_device("sync");
    if (!demo.rocket) {
        fprintf(stderr, "sync_create_device failed\n");
        demo_deinit();
        return EXIT_FAILURE;
    }

#ifndef SYNC_PLAYER
    // connect rocket
    while (sync_tcp_connect(demo.rocket, "localhost", SYNC_DEFAULT_PORT)) {
        printf("Waiting for Rocket editor...\n");
        sleep(2);
    }
#endif

    // store resolution
    demo.width = width;
    demo.height = height;

    // init scene
    if (demo_reload()) {
        demo_deinit();
        return EXIT_FAILURE;
    }

    // start music
    demo.player = player;
    SDL_PauseAudioDevice(player->audio_device, 0);

    return EXIT_SUCCESS;
}

void demo_deinit(void) {
#ifdef DEMO_RTDL
    if (demo.module) {
        if (demo.scene_deinit) {
            demo.scene_deinit(demo.scene_data);
        }
        dlclose(demo.module);
    }
#else
    scene_deinit(demo.scene_data);
#endif

    if (demo.rocket) {
#ifndef SYNC_PLAYER
        // save tracks to librocket-player format
        sync_save_tracks(demo.rocket);
#endif
        sync_destroy_device(demo.rocket);
    }

    // set stuff to null so next time we know what's uninitialized
    memset(&demo, 0, sizeof(struct demo_));
}

void demo_render(void) {
    // get time from player and convert to rocket row
    player_t *player = demo.player;
    SDL_LockAudioDevice(player->audio_device);
    double byte_at = player->playback.pos;
    SDL_UnlockAudioDevice(player->audio_device);
    demo.time = byte_at / player->spec.channels / sizeof(Uint16)
        / player->spec.freq;
    // add precision
    if (SDL_GetAudioDeviceStatus(player->audio_device) == SDL_AUDIO_PLAYING) {
        demo.time += (SDL_GetTicks() - player->playback.call_time) / 1000.;
    }
    demo.row = demo.time * demo.player->row_rate;

#ifndef SYNC_PLAYER
    // update rocket
    if (sync_update(demo.rocket, (int)demo.row, &player_cb, (void*)player)) {
        while (sync_tcp_connect(demo.rocket, "localhost", SYNC_DEFAULT_PORT)) {
            fprintf(stderr, "Attempting to reconnect\n");
            sleep(2);
        }
    }
#endif

#ifdef DEMO_RTDL
    demo.scene_render(demo.time, demo.scene_data);
#else
    scene_render(demo.time, demo.scene_data);
#endif
}

int demo_reload(void) {
#ifdef DEMO_RTDL
    // clean up if needed
    if (demo.scene_deinit && demo.scene_data) {
        demo.scene_deinit(demo.scene_data);
    }
    if (demo.module) {
        dlclose(demo.module);
    }

    // generate temporary filename
    char *tmp_file_path = tmpnam(NULL);

    // copy module file to temporary file
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/cp", "cp", MODULE_PATH, tmp_file_path, (char*)NULL);
    } else if (pid < 0) {
        fprintf(stderr, "Calling execl /bin/cp failed\n");
        return EXIT_FAILURE;
    } else {
        wait(NULL);
    }

    // load scene module from temporary file
    demo.module = dlopen(tmp_file_path, RTLD_LAZY);
    if (!demo.module) {
        fprintf(stderr, "%s\n", dlerror());
        return EXIT_FAILURE;
    }

    // unlink (remove) temporary file
    unlink(tmp_file_path);

    // load scene api
    *(void**)(&demo.scene_init) = dlsym(demo.module, "scene_init");
    *(void**)(&demo.scene_deinit) = dlsym(demo.module, "scene_deinit");
    *(void**)(&demo.scene_render) = dlsym(demo.module, "scene_render");
    if (!demo.scene_init || !demo.scene_deinit || !demo.scene_render) {
        fprintf(stderr, "Can't load symbols from module\n");
        dlclose(demo.module);
        return EXIT_FAILURE;
    }

    // init scene
    demo.scene_data = demo.scene_init(
            demo.width,
            demo.height,
            demo_sync_get_track,
            demo_sync_get_value);

    printf("Scene module loaded\n");
#else // ifdef DEMO_RTDL
    if (demo.scene_data) {
        scene_deinit(demo.scene_data);
    }
    demo.scene_data = scene_init(
            demo.width,
            demo.height,
            demo_sync_get_track,
            demo_sync_get_value);
#endif

    if (!demo.scene_data) {
        fprintf(stderr, "scene_init returned NULL\n");
#ifdef DEMO_RTDL
        dlclose(demo.module);
#endif
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

double demo_sync_get_value(const void *track) {
    return sync_get_val(track, demo.row);
}

const void *demo_sync_get_track(const char *name) {
    return sync_get_track(demo.rocket, name);
}
