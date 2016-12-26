########################################
# SOURCES
########################################

set(PARSER_INDEPENDENT_SOURCES
  constant-directive.cpp
  enhance-symbol-table-immutable-arguments.cpp
  execute-immutable-arguments.cpp
  expression-compiler-clike.cpp
  expression-tokenizer.cpp
  integer-parser.cpp
  intermediate-directive.cpp
  intermediate-instruction.cpp
  intermediate-macro-instruction.cpp
  intermediate-operation.cpp
  intermediate-representator.cpp
  macro-directive.cpp
  macro-directive-table.cpp
  macro-end-directive.cpp
  memory-allocator.cpp
  memory-definition-directive.cpp
  memory-reservation-directive.cpp
  memory-section-definition.cpp
  positioned-string.cpp
  preprocessing-immutable-arguments.cpp
  relative-memory-position.cpp
  section-directive.cpp
  section-tracker.cpp
  string-parser.cpp
  symbol.cpp
  symbol-graph.cpp
  symbol-graph-evaluation.cpp
  symbol-replacer.cpp
  syntax-tree-generator.cpp
  transformation-parameters.cpp
)

########################################
# TARGET
########################################

if (ERA_SIM_BUILD_PARSER)
  add_library(era-sim-parser-independent STATIC ${PARSER_INDEPENDENT_SOURCES})
  target_link_libraries(era-sim-parser-independent era-sim-common era-sim-arch-common era-sim-parser-common)
endif()
