########################################
# SOURCES
########################################

set(COMMON_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/utility.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-common STATIC ${COMMON_SOURCES})
target_compile_options(era-sim-common PUBLIC "-std=c++14")
