#define _POSIX_C_SOURCE 2

#include "demo.h"
#include "player.h"
#include <SDL.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef MONOLITH
#include "music.h"
#endif

#ifndef GL_PROFILE
#define GL_PROFILE CORE
#endif
#ifndef GL_MAJOR
#define GL_MAJOR 3
#endif
#ifndef GL_MINOR
#define GL_MINOR 3
#endif
#define SDL_GL_PROFILE__(prof) SDL_GL_CONTEXT_PROFILE_##prof
#define SDL_GL_PROFILE_(prof) SDL_GL_PROFILE__(prof)
#define SDL_GL_PROFILE SDL_GL_PROFILE_(GL_PROFILE)

static void arg_error(char option) {
    fprintf(stderr, "Bad value for option -%c\n", option);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int ret = 0;
    // parse arguments
    int opt, width = 640, height = 360, fs = 0, bpm = 120, rpb = 8,
             gl_major = GL_MAJOR, gl_minor = GL_MINOR, gl = SDL_GL_PROFILE;
    while ((opt = getopt(argc, argv, "w:h:b:r:a:i:ecmf")) != -1) {
        switch (opt) {
        case 'w':
            if ((width = atoi(optarg)) < 1)
                arg_error(opt);
            break;
        case 'h':
            if ((height = atoi(optarg)) < 1)
                arg_error(opt);
            break;
        case 'b':
            if ((bpm = atoi(optarg)) < 1)
                arg_error(opt);
            break;
        case 'r':
            if ((rpb = atoi(optarg)) < 1)
                arg_error(opt);
            break;
        case 'a':
            if ((gl_major = atoi(optarg)) < 1)
                arg_error(opt);
            break;
        case 'i':
            if ((gl_minor = atoi(optarg)) < 0)
                arg_error(opt);
            break;
        case 'e':
            gl = SDL_GL_CONTEXT_PROFILE_ES;
            break;
        case 'c':
            gl = SDL_GL_CONTEXT_PROFILE_CORE;
            break;
        case 'm':
            gl = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
            break;
        case 'f':
            fs = !fs;
            break;
        default:
            return EXIT_FAILURE;
        }
    }

    // start sdl video (+events) and audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        fprintf(stderr, "SDL2 failed to initialize: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // decode vorbis music and prepare player
    player_t player;
#ifdef MONOLITH
    ret = player_init_memory(&player, music_ogg, music_ogg_len);
#else
    ret = player_init_file(&player, "music.ogg");
#endif
    if (ret != 0) {
        return EXIT_FAILURE;
    }

    // opengl attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

    // get an opengl window
    SDL_Window *window = SDL_CreateWindow(
        (optind < argc ? argv[optind] : "-"), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, width, height,
        SDL_WINDOW_OPENGL | (fs ? SDL_WINDOW_FULLSCREEN : 0) |
            SDL_WINDOW_RESIZABLE);
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

    SDL_GL_GetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, &ret);
    if (!ret) {
        fprintf(stderr, "No sRGB framebuffer\n");
    }
    ret = 0;

    // connect/init rocket and prepare demo for rendering
    if (demo_init(&player, width, height, bpm, rpb)) {
        fprintf(stderr, "Demo failed to initialize\n");
        return EXIT_FAILURE;
    }

    SDL_Event e;
    int running = 1;

    // Set up frame rate counting
#ifdef DEBUG
    const unsigned FRAME_RATE_INTERVAL = 2000;
    unsigned frames = 0;
    unsigned time = SDL_GetTicks();
#endif

    while (running) {
#ifndef DEBUG
        running = !player_at_end(&player);
#endif

        // get sdl events like keyboard or kill signals
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE ||
                    e.key.keysym.sym == SDLK_q) {
                    running = 0;
                }
#ifdef DEBUG
                else if (e.key.keysym.sym == SDLK_r) {
                    if (demo_reload()) {
                        running = 0;
                    }
                }
#endif
            } else if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    demo_resize(e.window.data1, e.window.data2);
                }
            }
        }

        // render and show to screen
        demo_render();
        SDL_GL_SwapWindow(window);

#ifdef DEBUG // handle frame rate counting
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
    player_destroy(&player);
    SDL_Quit();
    return EXIT_SUCCESS;
}
