########################################
# SOURCES
########################################

set(COMMON_SOURCES
)

########################################
# TARGET
########################################

add_library(era-sim-common STATIC ${ARCH_SOURCES})
target_compile_options(era-sim-common PUBLIC "-std=c++14")
