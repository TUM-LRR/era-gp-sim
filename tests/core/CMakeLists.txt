########################################
# SOURCES
########################################

set(TEST_CORE_SOURCES
  memory-value-test.cpp
  task-scheduler-test.cpp
  # queue-test.cpp # Sorry, they did fail sometimes. :(
)

########################################
# TARGET
########################################

add_executable(era-sim-core-tests ${TEST_CORE_SOURCES})

target_link_libraries(era-sim-core-tests era-sim-core era-sim-common)
target_link_libraries(era-sim-core-tests gtest gtest_main)

# test-name executable-name
add_test(era-sim-core-tests era-sim-core-tests)
