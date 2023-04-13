# Mehustin is a system for building Linux demos using SDL2 and Rocket

This initializes an SDL2 window with OpenGL support, after which it
decodes music for playback using `stb_vorbis`.
Then it loads a libdemo.so -module (debug builds only), calls it's `scene_init`
function and starts calling `scene_render` in a loop. It also checks for
keyboard inputs and swaps the window.

Release builds don't (re)load the module dynamically.

## Dependencies

- GNU make
- pkg-config
- C99 compiler (clang and gcc tested)
- SDL2
- OpenGL (3.3 core by default)

For release builds of any kind:
- sstrip from elfkickers

For monolithic builds:
- xxd from vim
- shader_minifier.exe by Ctrl-Alt-Test

## Build instructions

First time setup:
```
git submodule update --init
make libs
```

Debugging and editing:
```
make
```
Run with `MESA_DEBUG=1 ./debug/mehustin` and reload code with R key.

Release (separate files):
```
make DEBUG=0
make DEBUG=0 install
```
A release directory with source code will be built (directory `demo`)

Release (single executable):
```
make DEBUG=0 monolith
```
A release binary will be built (`release/demo`)
