########################################
# SOURCES
########################################

set(PARSER_FACTORY_SOURCES
  parser-factory.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-parser-factory STATIC ${PARSER_FACTORY_SOURCES})
target_link_libraries(era-sim-parser-factory era-sim-common era-sim-arch-common era-sim-parser-common era-sim-parser-riscv)
