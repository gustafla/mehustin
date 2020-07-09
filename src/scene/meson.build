glesv2 = dependency('glesv2')
cc = meson.get_compiler('c')
m = cc.find_library('m', required : false)
sources = files(
  'scene.c',
  'obj.c',
  'read_file.c',
  'vec.c',
)
library('demo', sources, dependencies: [glesv2, m])
