########################################
# SOURCES
########################################

set(ARCH_RISCV_SOURCES
  control-flow-instructions.cpp
  instruction-node.cpp
  integer-instructions.cpp
  load-store-instructions.cpp
  mul-div-instructions.cpp
  instruction-node-factory.cpp
  immediate-node-factory.cpp
  register-access-node-factory.cpp
  instruction-node-factory.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-riscv STATIC ${ARCH_RISCV_SOURCES})

target_link_libraries(era-sim-arch-riscv era-sim-common era-sim-arch-common)
