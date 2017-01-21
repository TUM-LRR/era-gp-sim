########################################
# SOURCES
########################################

set(TEST_SYSTEM_SOURCES
  program-execution-test.cpp
)

########################################
# TARGET
########################################

  add_executable(era-sim-system-tests ${TEST_SYSTEM_SOURCES})
  add_test(
    NAME era-sim-system-tests
    COMMAND era-sim-system-tests
    WORKING_DIRECTORY ${ERA_SIM_BINARY_DIR}
  )

  target_link_libraries(era-sim-system-tests era-sim-core)
  target_link_libraries(era-sim-system-tests gtest gtest_main)
