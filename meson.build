project('openpower-libhei', 'cpp',
        version: '0.1', meson_version: '>=0.49.0',
        default_options: [
          'warning_level=3',
          'werror=true',
          'cpp_std=c++17'
        ])
subdir('sim')


executable('openpower-libhei',
           'hei_isolator.cpp',
           install : true)
