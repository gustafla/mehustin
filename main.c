#include <stdlib.h>
#include <SDL.h>
#include "player.h"
#include "demo.h"
#include <GL/gl.h>

int main(int argc, char **argv) {
    // parse arguments
    int width = 640, height = 360, srgb = 1;
    while (*(++argv)) {
        if (strcmp(*argv, "-w") == 0) {
            if (*(++argv) == NULL) goto arg_error;
            if ((width = atoi(*argv)) < 1) goto arg_error;
        } else if (strcmp(*argv, "-h") == 0) {
            if (*(++argv) == NULL) goto arg_error;
            if ((height = atoi(*argv)) < 1) goto arg_error;
        } else if (strcmp(*argv, "--disable-srgb") == 0) {
            srgb = 0;
        } else goto arg_error;
    }

    // start sdl video (+events) and audio
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        fprintf(stderr, "SDL2 failed to initialize %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // decode vorbis music and prepare player
    player_t *player = player_init("music.ogg");
    if (!player) {
        fprintf(stderr, "Music player failed to initialize\n");
        return EXIT_FAILURE;
    }

    // opengl attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
    if (srgb) {
        SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    }

    // get an opengl window
    SDL_Window *window = SDL_CreateWindow(
            "Mehu | Assembly 2018",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_OPENGL
#ifndef DEBUG
            | SDL_WINDOW_FULLSCREEN
#endif
            );
    if (!window) {
        fprintf(stderr, "SDL2 failed to initialize a window: %s\n",
                SDL_GetError());
        fprintf(stderr, "Try --disable-srgb\n");
        return EXIT_FAILURE;
    }

    // get and enable an opengl context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        fprintf(stderr, "SDL2 failed to create OpenGL context %s\n",
                SDL_GetError());
        return EXIT_FAILURE;
    }

    // set sRGB conversion on for sRGB formats
    if (srgb) {
        glEnable(GL_FRAMEBUFFER_SRGB);
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
    if (*argv == NULL) {
        fprintf(stderr, "Missing extra argument for %s\n", *(--argv));
    } else {
        fprintf(stderr, "Bad argument: %s\n", *argv);
    }
    return EXIT_FAILURE;
}
