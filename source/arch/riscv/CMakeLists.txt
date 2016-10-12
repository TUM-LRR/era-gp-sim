########################################
# SOURCES
########################################

set(ARCH_RISCV_SOURCES
    instruction-node.cpp
    load-store-instructions.cpp
    mul-div-instructions.cpp
    instruction-node-factory.cpp
    immediate-node-factory.cpp
    register-node-factory.cpp
    instruction-node-factory.cpp
    formats.cpp
  factory-map.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-riscv STATIC ${ARCH_RISCV_SOURCES})

target_link_libraries(era-sim-arch-riscv era-sim-arch-common)
