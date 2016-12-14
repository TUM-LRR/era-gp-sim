########################################
# SOURCES
########################################

set(TEST_CORE_SOURCES
  memory-value-test.cpp
  task-scheduler-test.cpp
  conversion-test.cpp
  register-set-test.cpp
  memory-test.cpp
  project-test.cpp
  queue-test.cpp
)

########################################
# TARGET
########################################

if (ERA_SIM_BUILD_CORE)
  add_executable(era-sim-core-tests ${TEST_CORE_SOURCES})

  target_link_libraries(era-sim-core-tests era-sim-core)
  target_link_libraries(era-sim-core-tests gtest gtest_main)

  # test-name executable-name
  add_test(
    NAME era-sim-core-tests
    COMMAND era-sim-core-tests
    WORKING_DIRECTORY ${ERA_SIM_BINARY_DIR}
  )
endif()
