########################################
# SOURCES
########################################

set(COMMON_SOURCES
  # Only headers right now :P
)

########################################
# TARGET
########################################

add_library(era-sim-common STATIC ${COMMON_SOURCES})
target_compile_options(era-sim-common PUBLIC "-std=c++14")
