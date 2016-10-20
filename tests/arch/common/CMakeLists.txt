########################################
# SOURCES
########################################

set(TEST_ARCH_COMMON_SOURCES
  brewery-test.cpp
  general-test.cpp
)

########################################
# TARGET
########################################

add_executable(era-sim-arch-common-tests ${TEST_ARCH_COMMON_SOURCES})

target_link_libraries(era-sim-arch-common-tests era-sim-arch-common)
target_link_libraries(era-sim-arch-common-tests gtest gtest_main)

# test-name executable-name
add_test(era-sim-arch-common-tests era-sim-arch-common-tests)
