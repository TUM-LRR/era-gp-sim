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
  section-directive.cpp
  riscv-regex.cpp
  syntax-information.cpp
  syntax-tree-generator.cpp
  expression-compiler-clike.cpp
  parser-factory.cpp
  integer-parser.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-parser STATIC ${PARSER_SOURCES})

target_link_libraries(era-sim-parser era-sim-common era-sim-arch-common)
