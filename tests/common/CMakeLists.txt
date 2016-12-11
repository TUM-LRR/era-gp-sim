########################################
# SOURCES
########################################

set(TEST_COMMON_SOURCES
  common-utility-tests.cpp
  multiregex-test.cpp
  common-assert-tests.cpp
  common-string-conversions-tests.cpp
)

########################################
# TARGET
########################################

add_executable(era-sim-common-tests ${TEST_COMMON_SOURCES})

target_link_libraries(era-sim-common-tests era-sim-common)
target_link_libraries(era-sim-common-tests gtest gtest_main)

# test-name executable-name
add_test(era-sim-common-tests era-sim-common-tests)
