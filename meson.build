project(
    'fwInputViewer',
    'cpp',
    version: '0.1',
    default_options: ['buildtype=debugoptimized'],
)

deps = [dependency('raylib'), dependency('sfml-window')]
src = ['src/main.cpp', 'src/input.cpp', 'src/config.cpp', 'src/createitem.cpp']
executable('fwInputViewer', src, dependencies: deps)
