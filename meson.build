project('super-tux-brawl', 'c', default_options: ['default_library=shared'])

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : true)
sdl3_dep = dependency('sdl3', required: true)

src = [
  'src/main.c',
  'src/logger.c',
]

incdir = include_directories('src')

executable('super-tux-brawl', src, dependencies: [sdl3_dep, m_dep], include_directories: incdir)
