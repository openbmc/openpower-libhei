# Simulator sources
sim_libhei = [
    'simulator.cpp',
    'hei_user_interface.cpp',
    '../hei_user_defines.cpp'
]

# Isolator sources
iso_libhei = [
    '../../src/isolator/hei_isolator.cpp',
    '../../src/isolator/hei_isolation_node.cpp',
    '../../src/register/hei_hardware_register.cpp',
    '../../src/util/hei_bit_string.cpp',
]

# Test cases
test_libhei = [
    'sample_test_case.cpp',
]

gtest = dependency('gtest', main : true, required : false, method : 'system')

# Build simulator linked locally
if gtest.found()
    test('simulator',
         executable('simulator', sim_libhei, iso_libhei, test_libhei,
                    dependencies : gtest, include_directories: incdir))
endif
