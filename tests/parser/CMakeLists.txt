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

if (ERASIM_BUILD_PARSER)
  add_executable(erasim-parser-tests ${TEST_PARSER_SOURCES})
  add_test(
    NAME erasim-parser-tests
    COMMAND erasim-parser-tests
    WORKING_DIRECTORY ${ERASIM_BINARY_DIR}
  )

  target_link_libraries(erasim-parser-tests erasim-parser-common erasim-parser-factory erasim-parser-independent erasim-parser-riscv)
  target_link_libraries(erasim-parser-tests gtest gtest_main)
endif()
