project(
    'mehustin',
    'c',
    default_options: [
        'c_std=c99',
        'warning_level=2',
        'ROCKET_PLAYER=if-release'
    ],
)

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required: false)

rocket_inc = include_directories('lib/rocket/lib')
rocket_sources = files(
    'lib/rocket/lib/device.c',
    'lib/rocket/lib/track.c'
)
rocket_lib = shared_library('rocket', rocket_sources)
rocket_dep = declare_dependency(link_with: rocket_lib, include_directories: rocket_inc)

stb_inc = include_directories('lib/stb')
stb_dep = declare_dependency(include_directories: stb_inc)

scene_inc = include_directories('src/scene')
scene_dep = declare_dependency(include_directories: scene_inc)

subdir('src/base')
subdir('src/scene')
