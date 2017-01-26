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

if (ERASIM_BUILD_PARSER)
  add_library(erasim-parser-riscv STATIC ${PARSER_RISCV_SOURCES})
  target_link_libraries(erasim-parser-riscv erasim-common erasim-arch-common erasim-parser-common erasim-parser-independent)
endif()
