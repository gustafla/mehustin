sdl2_dep = dependency('sdl2')

dl_lib = library('dl')
dl_dep = declare_dependency(link_with: dl_lib)

sources = files(
  'main.c',
  'demo.c',
  'player.c',
  'lib/rocket/lib/device.c',
  'lib/rocket/lib/track.c',
)

executable('mehustin', sources, dependencies: [m_dep, scene_dep, sdl2_dep, rocket_dep, stb_dep, dl_dep])
