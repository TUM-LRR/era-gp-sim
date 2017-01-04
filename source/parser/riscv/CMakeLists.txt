########################################
# SOURCES
########################################

set(PARSER_RISCV_SOURCES
  riscv-directive-factory.cpp
  riscv-parser.cpp
  riscv-regex.cpp
)

########################################
# TARGET
########################################

if (ERA_SIM_BUILD_PARSER)
  add_library(era-sim-parser-riscv STATIC ${PARSER_RISCV_SOURCES})
  target_link_libraries(era-sim-parser-riscv era-sim-common era-sim-arch-common era-sim-parser-common era-sim-parser-independent)
endif()
