########################################
# SOURCES
########################################

set(ARCH_SOURCES
	common/register-information.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-arch STATIC ${ARCH_SOURCES})
target_compile_options(era-sim-arch PUBLIC "-std=c++14")
