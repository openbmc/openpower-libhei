# Simulator sources
sim_src = files('../sim_tracing.cpp', 'sim_hardware_access.cpp', 'simulator.cpp')

subdir('testcases')

# Build simulator linked locally
test(
    'simulator',
    executable(
        'simulator',
        sim_src,
        test_src,
        dependencies: gtest_dep,
        link_with: libhei_static,
        include_directories: [incdir, '.'],
    ),
)
