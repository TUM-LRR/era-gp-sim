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

if (ERASIM_BUILD_ARCH)
  add_executable(erasim-arch-common-tests ${TEST_ARCH_COMMON_SOURCES})

  target_link_libraries(erasim-arch-common-tests erasim-arch-common)
  target_link_libraries(erasim-arch-common-tests gtest gtest_main)

  add_test(
    NAME erasim-arch-common-tests
    COMMAND erasim-arch-common-tests
    WORKING_DIRECTORY ${ERASIM_BINARY_DIR}
  )
endif()
