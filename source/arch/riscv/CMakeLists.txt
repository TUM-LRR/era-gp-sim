########################################
# SOURCES
########################################

set(ARCH_RISCV_SOURCES
	control_flow_instructions.cpp
	instruction_node.cpp
	integer_instructions.cpp
	load_store_instructions.cpp
	mul_div_instructions.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-riscv STATIC ${ARCH_RISCV_SOURCES})
