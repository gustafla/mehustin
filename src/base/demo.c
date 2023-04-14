#define _POSIX_C_SOURCE 200809L

#include "demo.h"
#include "sync.h"
#include <SDL.h>
#include <stdint.h>
#include <unistd.h>

#ifdef DEMO_RTDL
#include <dlfcn.h>
#include <sys/wait.h>
#ifndef MODULE_PATH
#define MODULE_PATH "./libdemo.so"
#endif
#else
#include "src/scene/scene.h"
#endif

static struct demo_ {
    double time;
    double row;
    double row_rate;
    player_t *player;
    struct sync_device *rocket;
    int width;
    int height;
    void *scene_data;

    // scene module variables
#ifdef DEMO_RTDL
    void *module;
    // width, height, getval("...")
    void *(*scene_init)(int32_t, int32_t, const void *(*)(const char *),
                        double (*)(const void *));
    // data
    void (*scene_deinit)(void *);
    // time, data
    void (*scene_render)(double, void *);
#endif
} demo;

// rocket editor sync with music player
#ifndef SYNC_PLAYER
static void player_set_row(void *d, int row) {
    player_t *player = (player_t *)d;
    player_set_time(player, row / demo.row_rate);
}

static struct sync_cb player_cb = {
    (void (*)(void *, int))player_pause, // player.c
    player_set_row,                      // demo.c
    (int (*)(void *))player_is_playing   // player.c
};
#else
#ifdef MONOLITH
#include "sync_tracks.h"
void *rocket_open(const char *filename, const char *mode) {
    const char *str = sync_track_filenames;
    for (size_t i = 0; str[0] /* string not empty */; i++) {
        if (strcmp(str, filename) == 0) {
            return fmemopen((void *)sync_track_data[i], sync_track_lens[i],
                            mode);
        }
        str = strchr(str, 0) + 1;
    }

    return NULL;
}

static struct sync_io_cb iocb = {
    .open = rocket_open,
    .read = (size_t(*)(void *, size_t, size_t, void *))fread,
    .close = (int (*)(void *))fclose};
#endif // defined(MONOLITH)
#endif // !defined(SYNC_PLAYER)

int demo_init(player_t *player, int width, int height, double bpm, double rpb) {
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
#else
#ifdef MONOLITH
    sync_set_io_cb(demo.rocket, &iocb);
#endif // defined(MONOLITH)
#endif // !defined(SYNC_PLAYER)

    // store resolution
    demo.width = width;
    demo.height = height;

    // set row rate
    demo.row_rate = (bpm / 60.) * rpb;

    // init scene
    if (demo_reload()) {
        demo_deinit();
        return EXIT_FAILURE;
    }

    // start music
    demo.player = player;
    player_pause(player, 0);

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
    // get time from player
    player_t *player = demo.player;
    demo.time = player_get_time(player);
    demo.row = demo.time * demo.row_rate;

#ifndef SYNC_PLAYER
    // update rocket
    if (sync_update(demo.rocket, (int)demo.row, &player_cb, (void *)player)) {
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
        execl("/bin/cp", "cp", MODULE_PATH, tmp_file_path, (char *)NULL);
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
    *(void **)(&demo.scene_init) = dlsym(demo.module, "scene_init");
    *(void **)(&demo.scene_deinit) = dlsym(demo.module, "scene_deinit");
    *(void **)(&demo.scene_render) = dlsym(demo.module, "scene_render");
    if (!demo.scene_init || !demo.scene_deinit || !demo.scene_render) {
        fprintf(stderr, "Can't load symbols from module\n");
        dlclose(demo.module);
        return EXIT_FAILURE;
    }

    // init scene
    demo.scene_data = demo.scene_init(demo.width, demo.height,
                                      demo_sync_get_track, demo_sync_get_value);

    printf("Scene module loaded\n");
#else // ifdef DEMO_RTDL
    if (demo.scene_data) {
        scene_deinit(demo.scene_data);
    }
    demo.scene_data = scene_init(demo.width, demo.height, demo_sync_get_track,
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
