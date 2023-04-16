#include "sync.h"

#ifdef MONOLITH
#include "sync_tracks.h" // sync_track_filenames, sync_track_data, sync_track_lens
#include <stdlib.h>
#include <string.h>

struct rocket_io {
    const unsigned char *data;
    const unsigned char *end;
};

void *rocket_open(const char *filename, const char *mode) {
    if (!filename) {
        return NULL;
    }

    const char *str = sync_track_filenames;
    for (size_t i = 0; str[0] /* string not empty */; i++) {
        if (strcmp(str, filename) == 0) {
            struct rocket_io *io = malloc(sizeof(struct rocket_io));
            if (io == NULL) {
                break;
            }

            io->data = sync_track_data[i];
            io->end = io->data + sync_track_lens[i];

            return io;
        }

        str = strchr(str, 0) + 1;
    }

    return NULL;
}

size_t rocket_read(void *buffer, size_t size, size_t count, void *rocket_io) {
    if (!buffer || !rocket_io) {
        return 0;
    }

    struct rocket_io *io = rocket_io;
    size_t request = size * count;
    size_t available = io->end - io->data;
    size_t giving = request > available ? available : request;

    memcpy(buffer, io->data, giving);
    io->data += giving;

    return giving;
}

int rocket_close(void *rocket_io) {
    if (rocket_io) {
        free(rocket_io);
    }
    return 0;
}

struct sync_io_cb rocket_iocb = {
    .open = rocket_open, .read = rocket_read, .close = rocket_close};

#else // !defined(MONOLITH)
#include <stdio.h>

struct sync_io_cb rocket_iocb = {
    .open = (void *(*)(const char *, const char *))fopen,
    .read = (size_t(*)(void *, size_t, size_t, void *))fread,
    .close = (int (*)(void *))fclose};

#endif // defined(MONOLITH)
