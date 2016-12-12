########################################
# SOURCES
########################################

set(ARCH_RISCV_SOURCES
  instruction-node.cpp
  immediate-node-factory.cpp
  register-node-factory.cpp
  instruction-node-factory.cpp
  formats.cpp
  factory-map.cpp
  data-node-factory.cpp
  documentation-builder.cpp
  instruction-context-information.cpp
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
