#include "demo.h"
#include <GLES2/gl2.h>
#include <SDL.h>
#include <unistd.h>

#ifndef DEMO_MONOLITHIC
#include <dlfcn.h>
#else
#include "src/scene/scene.h"
#endif

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

demo_t *demo_init(player_t *player, int width, int height) {
	demo_t *demo = calloc(1, sizeof(demo_t));
	if (!demo) return NULL;

	// init rocket
	demo->rocket = sync_create_device("sync");
	if (!demo->rocket) {
		fprintf(stderr, "sync_create_device failed\n");
		demo_free(demo);
		return NULL;
	}

#ifndef SYNC_PLAYER
	// connect rocket
	while (sync_tcp_connect(demo->rocket, "localhost", SYNC_DEFAULT_PORT)) {
		printf("Waiting for Rocket editor...\n");
		sleep(2);
	}
#endif

	// set resolution
	demo->width = width;
	demo->height = height;
	glViewport(0, 0, width, height);

	// init scene
	if (demo_reload(demo)) {
		demo_free(demo);
		return NULL;
	}

	// start music
	demo->player = player;
	SDL_PauseAudioDevice(player->audio_device, 0);
	return demo;

}

void demo_free(demo_t *demo) {
#ifndef DEMO_MONOLITHIC
	if (demo->module) {
		if (demo->scene_free) {
			demo->scene_free();
		}
		dlclose(demo->module);
	}
#else
	scene_free();
#endif

	if (demo->rocket) {
#ifndef SYNC_PLAYER
		// save tracks to librocket-player format
		sync_save_tracks(demo->rocket);
#endif
		sync_destroy_device(demo->rocket);
	}
	free(demo);
}

void demo_render(demo_t *demo) {
#ifdef DEBUG
	// check opengl errors
	switch (glGetError()) {
		case GL_NO_ERROR: break;
		case GL_INVALID_ENUM: fprintf(stderr, "GL_INVALID_ENUM\n"); break;
		case GL_INVALID_VALUE: fprintf(stderr, "GL_INVALID_VALUE\n"); break;
		case GL_INVALID_OPERATION: fprintf(stderr, "GL_INVALID_OPERATION\n"); break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: fprintf(stderr, "GL_INVALID_FRAMEBUFFER_OPERATION\n"); break;
		case GL_OUT_OF_MEMORY: fprintf(stderr, "GL_OUT_OF_MEMORY\n"); break;
		default: fprintf(stderr, "Unknown glGetError() return value\n");
	}
#endif

	// get time from player and convert to rocket row
	player_t *player = demo->player;
	SDL_LockAudioDevice(player->audio_device);
	double byte_at = player->playback.pos;
	SDL_UnlockAudioDevice(player->audio_device);
	demo->time = byte_at / player->spec.channels / sizeof(Uint16)
		/ player->spec.freq;
	demo->row = demo->time * demo->player->row_rate;

#ifndef SYNC_PLAYER
	// update rocket
	if (sync_update(demo->rocket, (int)demo->row, &player_cb, (void*)player)) {
		sync_tcp_connect(demo->rocket, "localhost", SYNC_DEFAULT_PORT);
	}
#endif

#ifndef DEMO_MONOLITHIC
	demo->scene_render();
#else
	scene_render();
#endif
}

int demo_reload(demo_t *demo) {
#ifndef DEMO_MONOLITHIC
	// clean up if needed
	if (demo->scene_free) {
		demo->scene_free();
	}
	if (demo->module) {
		dlclose(demo->module);
	}

	// load scene module
	demo->module = dlopen("./demo.so", RTLD_LAZY);
	if (!demo->module) {
		fprintf(stderr, "%s\n", dlerror());
		return EXIT_FAILURE;
	}

	// load scene api
	*(void**)(&demo->scene_init) = dlsym(demo->module, "scene_init");
	if (!demo->scene_init) goto module_error;
	*(void**)(&demo->scene_free) = dlsym(demo->module, "scene_free");
	if (!demo->scene_free) goto module_error;
	*(void**)(&demo->scene_render) = dlsym(demo->module, "scene_render");
	if (!demo->scene_render) goto module_error;

	// init scene
	if (demo->scene_init(demo)) {
		return EXIT_FAILURE;
	}

	printf("Scene module loaded\n");
#else
	scene_free();
	scene_init(demo);
#endif

	return EXIT_SUCCESS;

#ifndef DEMO_MONOLITHIC
module_error:
	fprintf(stderr, "%s\n", dlerror());
	dlclose(demo->module);
	return EXIT_FAILURE;
#endif
}

double demo_sync_get_value(const demo_t *demo, const char *name) {
	// ugly because sync_get_track takes long, should keep tracks cached
	return sync_get_val(sync_get_track(demo->rocket, name), demo->row);
}
