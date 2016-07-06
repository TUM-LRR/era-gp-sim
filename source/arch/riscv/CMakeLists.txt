########################################
# SOURCES
########################################

set(ARCH_RISCV_SOURCES
	control-flow-instructions.cpp
	instruction-node.cpp
	integer-instructions.cpp
	load-store-instructions.cpp
	mul-div-instructions.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-riscv STATIC ${ARCH_RISCV_SOURCES})
