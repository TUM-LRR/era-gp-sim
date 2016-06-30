########################################
# SOURCES
########################################

set(TEST_ARCH_COMMON_SOURCES
  arch-common-tests.cpp
)

########################################
# TARGET
########################################

add_executable(era-sim-arch-common-tests ${TEST_ARCH_COMMON_SOURCES})

target_link_libraries(era-sim-arch-common-tests era-sim-arch-common)
#target_link_libraries(era-sim-arch-common-tests getst gtests_main)

add_test(era-sim-arch-common-tests era-sim-arch-common-tests)
