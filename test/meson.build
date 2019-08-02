# build simulator
subdir('simulator')

# supporting files that need compiled/linked
test_sources = ['../src/hei_bit_string.cpp']

# build g-test framework unit tests
gtests = ['bit_string_test']

gtest = dependency('gtest', main : true, required : false, method : 'system')

if gtest.found()
    foreach g : gtests
        test(g, executable(g.underscorify(), g + '.cpp', '../src/util/hei_bit_string.cpp', \
                            dependencies : gtest, \
                             include_directories : incdir))
  endforeach
endif
