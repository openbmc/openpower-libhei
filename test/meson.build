# build simulator
subdir('simulator')

# build unit tests
tests = [
  'bitstring_test'
]

foreach t : tests
  test(t, executable(t.underscorify(), t + '.cpp', \
                    include_directories : incdir))
endforeach

# build g-test framework unit tests
gtests = [
  'bitstring_gtest'
]

gtest = dependency('gtest', main : true, required : false, method : 'system')

if gtest.found()
  foreach g : gtests
    test(g, executable(g.underscorify(), g + '.cpp', \
                      dependencies : gtest, \
                      include_directories : incdir))
  endforeach
endif
