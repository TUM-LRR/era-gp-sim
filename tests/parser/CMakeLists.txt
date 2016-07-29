########################################
# SOURCES
########################################

set(TEST_PARSER_SOURCES
  intermediate-instruction-test.cpp
  intermediate-representator-test.cpp
  symbol-table-test.cpp
  riscv-parser-test.cpp
)

########################################
# TARGET
########################################

add_executable(era-sim-parser-tests ${TEST_PARSER_SOURCES})

target_link_libraries(era-sim-parser-tests era-sim-parser era-sim-common)
target_link_libraries(era-sim-parser-tests gtest gtest_main)

add_test(era-sim-parser-tests era-sim-parser-tests)
