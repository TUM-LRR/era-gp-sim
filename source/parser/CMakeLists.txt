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
  memory-reservation-directive.cpp
  macro-directive.cpp
  macro-end-directive.cpp
  expression-compiler-clike.cpp
  parser-factory.cpp
  riscv-directive-factory.cpp
  integer-parser.cpp
  constant-directive.cpp
  compile-state.cpp
  final-representation.cpp
  memory-allocator.cpp
  macro-directive-table.cpp
  memory-definition-directive.cpp
  intermediate-macro-instruction.cpp
  intermediate-directive.cpp
  relative-memory-position.cpp
  memory-section-definition.cpp
  code-position.cpp
  line-interval.cpp
  compile-error.cpp
)

########################################
# TARGET
########################################

if (ERA_SIM_BUILD_PARSER)
  add_library(era-sim-parser STATIC ${PARSER_SOURCES})
  target_link_libraries(era-sim-parser era-sim-common era-sim-arch-common)
endif()
