set (PARSER_SOURCES
  intermediate-instruction.cpp
  intermediate-operation.cpp
  intermediate-representator.cpp
  parser.cpp
  riscv-parser.cpp
  symbol-table.cpp
)

add_library(era-sim-parser STATIC ${PARSER_SOURCES})