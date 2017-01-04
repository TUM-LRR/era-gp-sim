########################################
# SOURCES
########################################

set(TEST_PARSER_SOURCES
  expression-compiler-test.cpp
  integer-parser-test.cpp
  intermediate-instruction-test.cpp
  intermediate-representator-test.cpp
  memory-allocator-test.cpp
  parser-factory-test.cpp
  riscv-parser-test.cpp
  string-parser-test.cpp
  symbol-table-test.cpp
  syntax-information-test.cpp
  syntax-tree-generator-test.cpp
)

########################################
# TARGET
########################################

if (ERA_SIM_BUILD_PARSER)
  add_executable(era-sim-parser-tests ${TEST_PARSER_SOURCES})
  add_test(
    NAME era-sim-parser-tests
    COMMAND era-sim-parser-tests
    WORKING_DIRECTORY ${ERA_SIM_BINARY_DIR}
  )

  target_link_libraries(era-sim-parser-tests era-sim-parser-common era-sim-parser-factory era-sim-parser-independent era-sim-parser-riscv)
  target_link_libraries(era-sim-parser-tests gtest gtest_main)
endif()
