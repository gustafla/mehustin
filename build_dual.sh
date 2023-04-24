#!/bin/sh

# Clean up for mingw
rm -rf lib/rocket
git submodule update --init
x86_64-w64-mingw32-make clean

# Build Windows
x86_64-w64-mingw32-make libs
x86_64-w64-mingw32-make DEBUG=0 MINGW=1
x86_64-w64-mingw32-make DEBUG=0 MINGW=1 install
cp /usr/x86_64-w64-mingw32/bin/SDL2.dll demo/bin
cp /usr/x86_64-w64-mingw32/bin/glew32.dll demo/bin
cp /usr/x86_64-w64-mingw32/bin/libssp-0.dll demo/bin

# Restore linux development environment
rm -rf lib/rocket
git submodule update --init
make clean
make libs

# Build Linux
make DEBUG=0
make DEBUG=0 install
cp -L /usr/lib/libSDL2-2.0.so.0 demo/bin
