#!/bin/sh

# Clean up for mingw
rm -rf lib/rocket
git submodule update --init
x86_64-w64-mingw32-make clean

# Build
x86_64-w64-mingw32-make libs
x86_64-w64-mingw32-make DEBUG=0 MINGW=1 monolith
x86_64-w64-mingw32-make -f monolithic.mk DEBUG=0 MINGW=1 install
cp /usr/x86_64-w64-mingw32/bin/SDL2.dll demo
cp /usr/x86_64-w64-mingw32/bin/glew32.dll demo
cp /usr/x86_64-w64-mingw32/bin/libssp-0.dll demo

# Restore linux development environment
rm -rf lib/rocket
git submodule update --init
make clean
make libs
