########################################
# SOURCES
########################################

set(TEST_ARCH_COMMON_SOURCES
  arch-common-tests.cpp
  arch-common-brewery-tests.cpp
)

########################################
# TARGET
########################################

add_executable(era-sim-arch-common-tests ${TEST_ARCH_COMMON_SOURCES})

target_link_libraries(era-sim-arch-common-tests era-sim-arch-common era-sim-arch-riscv era-sim-core era-sim-common)
target_link_libraries(era-sim-arch-common-tests gtest gtest_main)

# test-name executable-name
add_test(era-sim-arch-common-tests era-sim-arch-common-tests)
