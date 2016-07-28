########################################
# SOURCES
########################################

set(ARCH_RISCV_SOURCES
	control-flow-instructions.cpp
	instruction-node.cpp
	integer-instructions.cpp
	load-store-instructions.cpp
	mul-div-instructions.cpp
        riscv-node-factories.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-riscv STATIC ${ARCH_RISCV_SOURCES})

target_link_libraries(era-sim-arch-riscv era-sim-arch-common era-sim-core)
