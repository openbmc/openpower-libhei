sim_sources = [
    'hei_sim_main.cpp',
    'hei_sim_user_interface.cpp',
    '../../src/isolator/hei_isolator.cpp'
]

sim = executable('simulator', sim_sources, \
                    include_directories: incdir)

test('simulator', sim)

