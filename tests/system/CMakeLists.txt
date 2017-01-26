########################################
# SOURCES
########################################

set(TEST_SYSTEM_SOURCES
  program-execution-test.cpp
)

########################################
# TARGET
########################################

  add_executable(erasim-system-tests ${TEST_SYSTEM_SOURCES})
  add_test(
    NAME erasim-system-tests
    COMMAND erasim-system-tests
    WORKING_DIRECTORY ${ERASIM_BINARY_DIR}
  )

  target_link_libraries(erasim-system-tests erasim-core)
  target_link_libraries(erasim-system-tests gtest gtest_main)
