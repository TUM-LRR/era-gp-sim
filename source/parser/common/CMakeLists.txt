########################################
# SOURCES
########################################

set(PARSER_COMMON_SOURCES
  code-position.cpp
  code-position-interval.cpp
  compile-error.cpp
  compile-error-list.cpp
  final-command.cpp
  final-representation.cpp
  macro-information.cpp
  parser.cpp
  syntax-information.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-parser-common STATIC ${PARSER_COMMON_SOURCES})
target_link_libraries(era-sim-parser-common era-sim-common era-sim-arch-common)
