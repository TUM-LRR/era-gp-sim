set (TEST_PARSER_SOURCES
  intermediate-instruction-unittest.cpp
  intermediate-representator-unittest.cpp
  symbol-table-unittest.cpp
)

add_executable(era-sim-parser-tests ${TEST_PARSER_SOURCES})
target_link_libraries(era-sim-parser-tests era-sim-parser gtest gtest_main)

add_test(era-sim-parser-tests era-sim-parser-tests)