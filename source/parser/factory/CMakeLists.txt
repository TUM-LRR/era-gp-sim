########################################
# SOURCES
########################################

set(PARSER_FACTORY_SOURCES
  parser-factory.cpp
)

########################################
# TARGET
########################################

add_library(erasim-parser-factory STATIC ${PARSER_FACTORY_SOURCES})
target_link_libraries(erasim-parser-factory erasim-common erasim-arch-common erasim-parser-common erasim-parser-riscv)
