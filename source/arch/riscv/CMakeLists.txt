########################################
# SOURCES
########################################

set(ARCH_RISCV_SOURCES
    control-flow-instructions.cpp
    instruction-node.cpp
    instruction-node-factory.cpp
    immediate-node-factory.cpp
    register-access-node-factory.cpp
    formats.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-riscv STATIC ${ARCH_RISCV_SOURCES})

target_link_libraries(era-sim-arch-riscv era-sim-arch-common)
