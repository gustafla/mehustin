#define _POSIX_C_SOURCE 2

#include <stdlib.h>
#include <unistd.h>
#include <SDL.h>
#include "player.h"
#include "demo.h"

static void arg_error(char option) {
    fprintf(stderr, "Bad value for option -%c\n", option);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    // parse arguments
    int opt, width = 640, height = 360, fs = 0, bpm = 120, rpb = 8,
        gl_major = 2, gl_minor = 0, gl = SDL_GL_CONTEXT_PROFILE_ES;
    while ((opt = getopt(argc, argv, "w:h:b:r:a:i:ecmf")) != -1) {
        switch (opt) {
            case 'w': if ((width = atoi(optarg)) < 1) arg_error(opt); break;
            case 'h': if ((height = atoi(optarg)) < 1) arg_error(opt); break;
            case 'b': if ((bpm = atoi(optarg)) < 1) arg_error(opt); break;
            case 'r': if ((rpb = atoi(optarg)) < 1) arg_error(opt); break;
            case 'a': if ((gl_major = atoi(optarg)) < 1) arg_error(opt); break;
            case 'i': if ((gl_minor = atoi(optarg)) < 0) arg_error(opt); break;
            case 'e': gl = SDL_GL_CONTEXT_PROFILE_ES; break;
            case 'c': gl = SDL_GL_CONTEXT_PROFILE_CORE; break;
            case 'm': gl = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY; break;
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
    player_t *player = player_init("music.ogg");
    if (!player) {
        return EXIT_FAILURE;
    }

    // opengl attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl);

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

    // Hide cursor in fullscreen mode
    if (fs) {
        SDL_ShowCursor(SDL_DISABLE);
    }

    // get and enable an opengl context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        fprintf(stderr, "SDL2 failed to create an OpenGL context: %s\n",
                SDL_GetError());
        return EXIT_FAILURE;
    }

    // connect/init rocket and prepare demo for rendering
    if (demo_init(player, width, height, bpm, rpb)) {
        fprintf(stderr, "Demo failed to initialize\n");
        return EXIT_FAILURE;
    }

    SDL_Event e;

    // Set up frame rate counting
#ifdef DEBUG
    const unsigned FRAME_RATE_INTERVAL = 2000;
    unsigned frames = 0;
    unsigned time = SDL_GetTicks();
#endif
#ifdef DEBUG
    while(1) {
#else
    while(!player_at_end(player)) {
#endif
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
                if (demo_reload()) {
                    break;
                }
            }
#endif
        }

        // render and show to screen
        demo_render();
        SDL_GL_SwapWindow(window);

        // handle frame rate counting
#ifdef DEBUG
        frames++;
        unsigned newtime = SDL_GetTicks();
        if (newtime >= time + FRAME_RATE_INTERVAL) {
            printf("Frame rate: %f\n", frames * 1000.f / FRAME_RATE_INTERVAL);
            frames = 0;
            time = newtime;
        }
#endif
    }

    // disconnect rocket and save tracks
    demo_deinit();
    player_free(player);
    SDL_Quit();
    return EXIT_SUCCESS;
}
