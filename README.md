# Mehustin is a Linux OpenGL ES 2.0 demo boilerplate using SDL2 and Rocket

The tool initializes an SDL2 window with OpenGL ES 2.0 support, after which it
decodes a music.ogg file for playback using `stb_vorbis`.
Then it loads a libdemo.so -module (debug builds only), calls it's `scene_init`
function and starts calling `scene_render` in a loop. It also checks for
keyboard inputs and swaps the window.

Release builds don't (re)load the module dynamically.
