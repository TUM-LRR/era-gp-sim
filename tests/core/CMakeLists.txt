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

if (ERASIM_BUILD_CORE)
  add_executable(erasim-core-tests ${TEST_CORE_SOURCES})

  target_link_libraries(erasim-core-tests erasim-core)
  target_link_libraries(erasim-core-tests gtest gtest_main)

  # test-name executable-name
  add_test(
    NAME erasim-core-tests
    COMMAND erasim-core-tests
    WORKING_DIRECTORY ${ERASIM_BINARY_DIR}
  )
endif()
