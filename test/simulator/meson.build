# Simulator sources
sim_src = [
    'simulator.cpp',
    'user_interface.cpp',
]

# Isolator sources
iso_src = [
    '../../src/isolator/hei_isolator.cpp',
    '../../src/isolator/hei_isolation_node.cpp',
    '../../src/register/hei_hardware_register.cpp',
    '../../src/util/hei_bit_string.cpp',
]

# Test cases
test_src = [
    'sample_test_case.cpp',
]

data_files = [
    'chip_data/chip_data_sample.cdb'
]

# Copy the Chip Data Files needed for the simulator. Note that the tests
# executables are run out the base build directory and not from the directory
# in which the executable is built.
foreach f : data_files
    run_command( 'cp', f, '../../build/' )
endforeach

gtest = dependency('gtest', main : true, required : false, method : 'system')

if gtest.found()
    test('simulator', \
         executable('simulator', sim_src, iso_src, test_src, \
                    dependencies : gtest, include_directories: incdir))
endif

