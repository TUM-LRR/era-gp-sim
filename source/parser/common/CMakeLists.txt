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
  syntax-information.cpp
)

########################################
# TARGET
########################################

add_library(erasim-parser-common STATIC ${PARSER_COMMON_SOURCES})
target_link_libraries(erasim-parser-common erasim-common erasim-arch-common)
