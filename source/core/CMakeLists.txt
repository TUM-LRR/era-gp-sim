########################################
# SOURCES
########################################

set(CORE_SOURCES
  memory-value.cpp
  conversions.cpp
)

########################################
# TARGET
########################################

add_library(era-sim-core STATIC ${CORE_SOURCES})
