#include <stdlib.h>
#include <unistd.h>
#include <SDL.h>
#include <GLES2/gl2.h>
#include "player.h"
#include "demo.h"

int main(int argc, char *argv[]) {
	// parse arguments
	int opt, width = 640, height = 360, fs = 0, bpm = 120, rpb = 8;
	while ((opt = getopt(argc, argv, "w:h:b:r:cf")) != -1) {
		switch (opt) {
			case 'w': if ((width = atoi(optarg)) < 1) goto arg_error; break;
			case 'h': if ((height = atoi(optarg)) < 1) goto arg_error; break;
			case 'b': if ((bpm = atoi(optarg)) < 1) goto arg_error; break;
			case 'r': if ((rpb = atoi(optarg)) < 1) goto arg_error; break;
			case 'f': fs = !fs; break;
			default: return EXIT_FAILURE;
		}
	}

	// start sdl video (+events) and audio
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
		fprintf(stderr, "SDL2 failed to initialize: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// decode vorbis music and prepare player
	player_t *player = player_init("music.ogg", bpm, rpb);
	if (!player) {
		return EXIT_FAILURE;
	}

	// opengl attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_ES);

	// get an opengl window
	SDL_Window *window = SDL_CreateWindow((optind < argc ? argv[optind] : "-"),
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
			SDL_WINDOW_OPENGL | (fs ? SDL_WINDOW_FULLSCREEN : 0));
	if (!window) {
		fprintf(stderr, "SDL2 failed to initialize a window: %s\n",
				SDL_GetError());
		fprintf(stderr, "Try the -c option\n");
		return EXIT_FAILURE;
	}

	// get and enable an opengl context
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	if (!gl_context) {
		fprintf(stderr, "SDL2 failed to create an OpenGL context: %s\n",
				SDL_GetError());
		return EXIT_FAILURE;
	}

	// connect/init rocket and prepare demo for rendering
	demo_t *demo = demo_init(player, width, height);
	if (!demo) {
		fprintf(stderr, "Demo failed to initialize\n");
		return EXIT_FAILURE;
	}

	SDL_Event e;
	while(1) {
		// get sdl events like keyboard or kill signals
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			break;
		} else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE || e.key.keysym.sym == SDLK_q) {
				break;
			}
#ifdef DEBUG
			else if (e.key.keysym.sym == SDLK_r) {
				if (demo_reload(demo)) {
					break;
				}
			}
#endif
		}

		// render and show to screen
		demo_render(demo);
		SDL_GL_SwapWindow(window);
	}

	// disconnect rocket and save tracks
	demo_free(demo);
	player_free(player);
	SDL_Quit();
	return EXIT_SUCCESS;

arg_error:
	fprintf(stderr, "bad value\n");
	return EXIT_FAILURE;
}
