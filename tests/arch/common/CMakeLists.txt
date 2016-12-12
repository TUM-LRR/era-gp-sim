########################################
# SOURCES
########################################

set(TEST_ARCH_COMMON_SOURCES
  brewery-test.cpp
  general-test.cpp
  unit-information-test.cpp
)

########################################
# TARGET
########################################

if (ERA_SIM_BUILD_ARCH)
  add_executable(era-sim-arch-common-tests ${TEST_ARCH_COMMON_SOURCES})

  target_link_libraries(era-sim-arch-common-tests era-sim-arch-common)
  target_link_libraries(era-sim-arch-common-tests gtest gtest_main)

  # test-name executable-name
  add_test(era-sim-arch-common-tests era-sim-arch-common-tests)
endif()
