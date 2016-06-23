########################################
# SOURCES
########################################

set(ARCH_COMMON_SOURCES
	register-information.cpp
	datatype-information.cpp
	unit-information.cpp
	extension-information.cpp
	architecture.cpp
	instruction-set.cpp
	instruction-information.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch-common STATIC ${ARCH_COMMON_SOURCES})
