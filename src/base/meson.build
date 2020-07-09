sdl2 = dependency('sdl2')

dl_lib = library('dl')
dl = declare_dependency(link_with: dl_lib)

sources = files(
  'main.c',
  'demo.c',
  'player.c',
)

executable('mehustin', sources, dependencies: [scene, sdl2, rocket, stb, dl])
