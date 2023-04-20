#include "demo.h"
#include "demo_procs.h"
#include "sync.h"
#include "sync_io.h"
#include <SDL.h>
#include <stdint.h>
#include <unistd.h>

static struct demo_ {
    double time;
    double row;
    double row_rate;
    player_t *player;
    struct sync_device *rocket;
    int width;
    int height;
    void *scene_data;
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
#endif // !defined(SYNC_PLAYER)

int demo_init(player_t *player, int width, int height, double bpm, double rpb) {
    // init rocket
    demo.rocket = sync_create_device("sync");
    if (!demo.rocket) {
        fprintf(stderr, "sync_create_device failed\n");
        demo_deinit();
        return EXIT_FAILURE;
    }

#ifdef SYNC_PLAYER
    // Set rocket file io callback (sync_io.h)
    sync_set_io_cb(demo.rocket, &rocket_iocb);
#else  // !defined(SYNC_PLAYER)
    // connect rocket
    while (sync_tcp_connect(demo.rocket, "localhost", SYNC_DEFAULT_PORT)) {
        printf("Waiting for Rocket editor...\n");
        sleep(2);
    }
#endif // defined(SYNC_PLAYER)

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
    SCENE_DEINIT(demo.scene_data);
    procs_deinit();

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

    SCENE_RENDER(demo.scene_data, demo.time);
}

int demo_reload(void) {
    SCENE_DEINIT(demo.scene_data);
    if (procs_reload()) {
        return EXIT_FAILURE;
    }

    demo.scene_data = SCENE_INIT(demo.width, demo.height, demo_sync_get_track,
                                 demo_sync_get_value);

    if (!demo.scene_data) {
        fprintf(stderr, "scene_init returned NULL\n");
        procs_deinit();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void demo_resize(int width, int height) {
    SCENE_RESIZE(demo.scene_data, width, height);
}

double demo_sync_get_value(const void *track) {
    return sync_get_val(track, demo.row);
}

const void *demo_sync_get_track(const char *name) {
    return sync_get_track(demo.rocket, name);
}
