########################################
# SOURCES
########################################

set(CORE_SOURCES
  memory-value.cpp
  register-set.cpp
  conversions.cpp
  project.cpp
  project-module.cpp
  parsing-and-execution-unit.cpp
  memory.cpp
  scheduler.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-core STATIC ${CORE_SOURCES})

target_link_libraries(era-sim-core era-sim-common era-sim-parser era-sim-arch-common era-sim-arch-riscv pthread)
