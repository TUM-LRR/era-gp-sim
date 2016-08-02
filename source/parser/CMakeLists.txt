########################################
# SOURCES
########################################

set(PARSER_SOURCES
  intermediate-instruction.cpp
  intermediate-operation.cpp
  intermediate-representator.cpp
  parser.cpp
  riscv-parser.cpp
  symbol-table.cpp
  riscv-regex.cpp
  syntax-tree-generator.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-parser STATIC ${PARSER_SOURCES})
