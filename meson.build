project(
    'mehustin',
    'c',
    default_options: [
        'c_std=c99',
        'warning_level=2',
    ],
)

rocket_inc = include_directories('lib/rocket/lib')
rocket_lib = static_library('rocket')
rocket = declare_dependency(
  link_with: rocket_lib,
  include_directories: rocket_inc
)

stb_inc = include_directories('lib/stb')
stb = declare_dependency(include_directories: stb_inc)

scene_inc = include_directories('src/scene')
scene = declare_dependency(include_directories: scene_inc)

subdir('src/base')
subdir('src/scene')
