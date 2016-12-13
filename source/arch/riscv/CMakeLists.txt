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
  register-node-factory.cpp
  register-node.cpp
  simulator-instructions.cpp
)

########################################
# TARGET
########################################

if (ERA_SIM_BUILD_ARCH)
  add_library(era-sim-arch-riscv STATIC ${ARCH_RISCV_SOURCES})
  target_link_libraries(
    era-sim-arch-riscv
    era-sim-common
    era-sim-arch-common
    era-sim-core
  )
endif()
