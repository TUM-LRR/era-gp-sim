########################################
# SOURCES
########################################

set(ARCH_RISCV_SOURCES
  data-node-factory.cpp
  documentation-builder.cpp
  factory-map.cpp
  format.cpp
  immediate-node-factory.cpp
  instruction-context-information.cpp
  instruction-node-factory.cpp
  instruction-node.cpp
  lui-auipc-instructions.cpp
  register-node-factory.cpp
  register-node.cpp
  simulator-instructions.cpp
)

########################################
# TARGET
########################################

if (ERASIM_BUILD_ARCH)
  add_library(erasim-arch-riscv STATIC ${ARCH_RISCV_SOURCES})
  target_link_libraries(
    erasim-arch-riscv
    erasim-common
    erasim-arch-common
    erasim-core
  )
endif()
